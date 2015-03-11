/* Copyright: (c) Kayne Ruse 2013-2015
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
#include "world.hpp"

#include "channels.hpp"

#include "terminal_error.hpp"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

//-------------------------
//Public access members
//-------------------------

World::World(int* const argClientIndex,	int* const argAccountIndex):
	clientIndex(*argClientIndex),
	accountIndex(*argAccountIndex)
{
	//setup the utility objects
	buttonImage.LoadSurface(config["dir.interface"] + "button_menu.bmp");
	buttonImage.SetClipH(buttonImage.GetClipH()/3);
	font.LoadSurface(config["dir.fonts"] + "pk_white_8.bmp");

	//pass the utility objects
	disconnectButton.SetImage(&buttonImage);
	disconnectButton.SetFont(&font);
	shutDownButton.SetImage(&buttonImage);
	shutDownButton.SetFont(&font);

	//set the button positions
	disconnectButton.SetX(50);
	disconnectButton.SetY(50 + buttonImage.GetClipH() * 0);
	shutDownButton.SetX(50);
	shutDownButton.SetY(50 + buttonImage.GetClipH() * 1);

	//set the button texts
	disconnectButton.SetText("Disconnect");
	shutDownButton.SetText("Shut Down");

	//load the tilesheet
	//TODO: (2) Tile size and tile sheet should be loaded elsewhere
	tileSheet.Load(config["dir.tilesets"] + "overworld.bmp", 32, 32);

	//Send the character data
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_LOAD;
	strncpy(newPacket.handle, config["client.handle"].c_str(), PACKET_STRING_SIZE);
	strncpy(newPacket.avatar, config["client.avatar"].c_str(), PACKET_STRING_SIZE);
	newPacket.accountIndex = accountIndex;
	network.SendTo(Channels::SERVER, &newPacket);

	//TODO: (1) replace this duplication with a request for just this player's character
	//query the world state
	memset(&newPacket, 0, MAX_PACKET_SIZE);
	newPacket.type = SerialPacketType::QUERY_CHARACTER_EXISTS;
	network.SendTo(Channels::SERVER, &newPacket);
	newPacket.type = SerialPacketType::QUERY_MONSTER_EXISTS;
	network.SendTo(Channels::SERVER, &newPacket);

	//set the camera's values
	camera.width = GetScreen()->w;
	camera.height = GetScreen()->h;

	//debug
	//
}

World::~World() {
	//unload the local data
	characterMap.clear();
	monsterMap.clear();
}

//-------------------------
//Frame loop
//-------------------------

void World::FrameStart() {
	//
}

void World::Update() {
	//create and zero the buffer
	SerialPacket* packetBuffer = reinterpret_cast<SerialPacket*>(new char[MAX_PACKET_SIZE]);
	memset(packetBuffer, 0, MAX_PACKET_SIZE);

	try {
		//suck in and process all waiting packets
		while(network.Receive(packetBuffer)) {
			HandlePacket(packetBuffer);
		}
	}
	catch(terminal_error& e) {
		throw(e);
	}
	catch(std::exception& e) {
		std::cerr << "HandlePacket Error: " << e.what() << std::endl;
	}

	//free the buffer
	delete reinterpret_cast<char*>(packetBuffer);

	//heartbeat system
	CheckHeartBeat();

	//update all entities
	for (auto& it : characterMap) {
		it.second.Update();
	}
	for (auto& it : monsterMap) {
		it.second.Update();
	}

	//update the map
	UpdateMap();

	//skip the rest without a local character
	if (!localCharacter) {
		return;
	}

	//get the collidable boxes
	std::list<BoundingBox> boxList = GenerateCollisionGrid(localCharacter, tileSheet.GetTileW(), tileSheet.GetTileH());

	//process the collisions
	if (localCharacter->ProcessCollisionGrid(boxList)) {
		localCharacter->CorrectSprite();
		SendLocalCharacterMovement();
	}

	//update the camera
	camera.x = localCharacter->GetOrigin().x - camera.marginX;
	camera.y = localCharacter->GetOrigin().y - camera.marginY;
}

void World::FrameEnd() {
	//
}

void World::RenderFrame() {
//	SDL_FillRect(GetScreen(), 0, 0);
	Render(GetScreen());
	SDL_Flip(GetScreen());
	fps.Calculate();
}

void World::Render(SDL_Surface* const screen) {
	//draw the map
	for (std::list<Region>::iterator it = regionPager.GetContainer()->begin(); it != regionPager.GetContainer()->end(); it++) {
		tileSheet.DrawRegionTo(screen, &(*it), camera.x, camera.y);
	}

	//draw the entities
	for (auto& it : characterMap) {
		//BUG: #29 Characters (and other entities) are drawn out of order
		it.second.DrawTo(screen, camera.x, camera.y);
	}
	for (auto& it : monsterMap) {
		it.second.DrawTo(screen, camera.x, camera.y);
	}

	//draw UI
	disconnectButton.DrawTo(screen);
	shutDownButton.DrawTo(screen);
	std::ostringstream msg;
	msg << fps.GetFrameRate();
	font.DrawStringTo(msg.str(), screen, 0, 0);
}

//-------------------------
//Event handlers
//-------------------------

void World::QuitEvent() {
	//two-step logout
	SendDisconnectRequest();
	SetNextScene(SceneList::QUIT);
}

void World::MouseMotion(SDL_MouseMotionEvent const& motion) {
	disconnectButton.MouseMotion(motion);
	shutDownButton.MouseMotion(motion);
}

void World::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	disconnectButton.MouseButtonDown(button);
	shutDownButton.MouseButtonDown(button);
}

void World::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (disconnectButton.MouseButtonUp(button) == Button::State::HOVER && button.button == SDL_BUTTON_LEFT) {
		SendLogoutRequest();
	}
	if (shutDownButton.MouseButtonUp(button) == Button::State::HOVER && button.button == SDL_BUTTON_LEFT) {
		SendAdminShutdownRequest();
	}
}

void World::KeyDown(SDL_KeyboardEvent const& key) {
	//hotkeys
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			//TODO: (3) the escape key should actually control menus and stuff
			SendLogoutRequest();
		return;
	}

	//character movement
	if (!localCharacter) {
		return;
	}
	Vector2 motion = localCharacter->GetMotion();
	switch(key.keysym.sym) {
		case SDLK_w:
			motion.y -= CHARACTER_WALKING_SPEED;
		break;
		case SDLK_a:
			motion.x -= CHARACTER_WALKING_SPEED;
		break;
		case SDLK_s:
			motion.y += CHARACTER_WALKING_SPEED;
		break;
		case SDLK_d:
			motion.x += CHARACTER_WALKING_SPEED;
		break;
		default:
			//DOCS: prevents wrong keys screwing with character movement
			return;
	}
	//handle diagonals
	if (motion.x != 0 && motion.y != 0) {
		motion *= CHARACTER_WALKING_MOD;
	}
	//set the info
	localCharacter->SetMotion(motion);
	localCharacter->CorrectSprite();
	SendLocalCharacterMovement();
}

void World::KeyUp(SDL_KeyboardEvent const& key) {
	//character movement
	if (!localCharacter) {
		return;
	}
	Vector2 motion = localCharacter->GetMotion();
	switch(key.keysym.sym) {
		case SDLK_w:
			motion.y = std::min(0.0, motion.y += CHARACTER_WALKING_SPEED);
		break;
		case SDLK_a:
			motion.x = std::min(0.0, motion.x += CHARACTER_WALKING_SPEED);
		break;
		case SDLK_s:
			motion.y = std::max(0.0, motion.y -= CHARACTER_WALKING_SPEED);
		break;
		case SDLK_d:
			motion.x = std::max(0.0, motion.x -= CHARACTER_WALKING_SPEED);
		break;
		default:
			//DOCS: prevents wrong keys screwing with character movement
			return;
	}
	//BUGFIX: reset cardinal direction speed on key release
	if (motion.x > 0) {
		motion.x = CHARACTER_WALKING_SPEED;
	}
	else if (motion.x < 0) {
		motion.x = -CHARACTER_WALKING_SPEED;
	}
	if (motion.y > 0) {
		motion.y = CHARACTER_WALKING_SPEED;
	}
	else if (motion.y < 0) {
		motion.y = -CHARACTER_WALKING_SPEED;
	}
	//handle diagonals
	if (motion.x != 0 && motion.y != 0) {
		motion *= CHARACTER_WALKING_MOD;
	}
	//set the info
	localCharacter->SetMotion(motion);
	localCharacter->CorrectSprite();
	SendLocalCharacterMovement();
}

//-------------------------
//Direct incoming traffic
//-------------------------

void World::HandlePacket(SerialPacket* const argPacket) {
	switch(argPacket->type) {
		//heartbeat system
		case SerialPacketType::PING:
			hPing(static_cast<ServerPacket*>(argPacket));
		break;
		case SerialPacketType::PONG:
			hPong(static_cast<ServerPacket*>(argPacket));
		break;

		//game server connections
		case SerialPacketType::LOGOUT_RESPONSE:
			hLogoutResponse(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::DISCONNECT_RESPONSE:
			hDisconnectResponse(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::ADMIN_DISCONNECT_FORCED:
			hAdminDisconnectForced(static_cast<ClientPacket*>(argPacket));
		break;

		//map management
		case SerialPacketType::REGION_CONTENT:
			hRegionContent(static_cast<RegionPacket*>(argPacket));
		break;

		//character management
		case SerialPacketType::CHARACTER_UPDATE:
			hCharacterUpdate(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_CREATE:
			hCharacterCreate(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_DELETE:
			hCharacterDelete(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_CHARACTER_EXISTS:
			hQueryCharacterExists(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_CHARACTER_STATS:
			hQueryCharacterStats(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_CHARACTER_LOCATION:
			hQueryCharacterLocation(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_MOVEMENT:
			hCharacterMovement(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_ATTACK:
			hCharacterAttack(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_DAMAGE:
			hCharacterDamage(static_cast<CharacterPacket*>(argPacket));
		break;

		//monster management
		case SerialPacketType::MONSTER_CREATE:
			hMonsterCreate(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::MONSTER_DELETE:
			hMonsterDelete(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_MONSTER_EXISTS:
			hQueryMonsterExists(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_MONSTER_STATS:
			hQueryMonsterStats(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_MONSTER_LOCATION:
			hQueryMonsterLocation(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::MONSTER_MOVEMENT:
			hMonsterMovement(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::MONSTER_ATTACK:
			hMonsterAttack(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::MONSTER_DAMAGE:
			hMonsterDamage(static_cast<MonsterPacket*>(argPacket));
		break;

		//chat
		case SerialPacketType::TEXT_BROADCAST:
			hTextBroadcast(static_cast<TextPacket*>(argPacket));
		break;
		case SerialPacketType::TEXT_SPEECH:
			hTextSpeech(static_cast<TextPacket*>(argPacket));
		break;
		case SerialPacketType::TEXT_WHISPER:
			hTextWhisper(static_cast<TextPacket*>(argPacket));
		break;

		//general rejection messages
		case SerialPacketType::REGION_REJECTION:
		case SerialPacketType::CHARACTER_REJECTION:
		case SerialPacketType::QUERY_REJECTION:
			throw(terminal_error(static_cast<TextPacket*>(argPacket)->text));
		break;
		case SerialPacketType::SHUTDOWN_REJECTION:
			throw(std::runtime_error(static_cast<TextPacket*>(argPacket)->text));
		break;

		//errors
		default: {
			std::ostringstream msg;
			msg << "Unknown SerialPacketType encountered in World: " << static_cast<int>(argPacket->type);
			throw(std::runtime_error(msg.str()));
		}
		break;
	}
}