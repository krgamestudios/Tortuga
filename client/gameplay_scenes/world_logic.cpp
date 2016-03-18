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
#include <cstring>
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
	buttonImage.Load(GetRenderer(), config["dir.interface"] + "button_blue.png");
	font = TTF_OpenFont(config["client.font"].c_str(), 12);

	//check that the font loaded
	if (!font) {
		std::ostringstream msg;
		msg << "Failed to load a font file; " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	//setup the buttons
	disconnectButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	disconnectButton.SetText(GetRenderer(), font, "Disconnect", COLOR_WHITE);
	shutdownButton.SetBackgroundTexture(GetRenderer(), buttonImage.GetTexture());
	shutdownButton.SetText(GetRenderer(), font, "Shutdown", COLOR_WHITE);

	//set the button positions
	disconnectButton.SetX(50);
	disconnectButton.SetY(50);
	shutdownButton.SetX(50);
	shutdownButton.SetY(70);

	//load the tilesheet
	//TODO: (2) Tile size and tile sheet should be loaded elsewhere
	tileSheet.Load(GetRenderer(), config["dir.tilesets"] + "overworld.png", 32, 32);

	//Send the character data
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_LOAD;
	strncpy(newPacket.handle, config["client.handle"].c_str(), PACKET_STRING_SIZE);
	strncpy(newPacket.avatar, config["client.avatar"].c_str(), PACKET_STRING_SIZE);
	newPacket.accountIndex = accountIndex;
	network.SendTo(Channels::SERVER, &newPacket);

	//set the camera's values
	SDL_RenderGetLogicalSize(GetRenderer(), &camera.width, &camera.height);

	//debug
	//
}

World::~World() {
	//unload the local data
	TTF_CloseFont(font);
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

	try {
		//update the map
		UpdateMap();
	}
	catch(terminal_error& e) {
		throw(e);
	}
	catch(std::exception& e) {
		std::cerr << "UpdateMap Error: " << e.what() << std::endl;
	}

	//skip the rest without a local character
	if (!localCharacter) {
		return;
	}

	//get the collidable boxes
	std::list<BoundingBox> boxList = GenerateCollisionGrid(localCharacter, tileSheet.GetTileW(), tileSheet.GetTileH());

	std::cout << "Debug: " << boxList.size() << std::endl;

	//process the collisions
	//BUG: Collisions not working
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

void World::RenderFrame(SDL_Renderer* renderer) {
	//draw the map
	for (std::list<Region>::iterator it = regionPager.GetContainer()->begin(); it != regionPager.GetContainer()->end(); it++) {
		tileSheet.DrawRegionTo(renderer, &(*it), camera.x, camera.y);

		//debugging
//		std::ostringstream msg;
//		msg << it->GetX() << ", " << it->GetY();
//		font.DrawStringTo(msg.str(), screen, it->GetX() * tileSheet.GetImage()->GetClipW() - camera.x, it->GetY() * tileSheet.GetImage()->GetClipH() - camera.y);
	}

	//draw the entities
	for (auto& it : characterMap) {
		//BUG: #29 Characters (and other entities) are drawn out of order
		it.second.DrawTo(renderer, camera.x, camera.y);
	}
	for (auto& it : monsterMap) {
		it.second.DrawTo(renderer, camera.x, camera.y);
	}

	//draw UI
	disconnectButton.DrawTo(renderer);
	shutdownButton.DrawTo(renderer);

	//FPS
	fpsTextLine.DrawTo(renderer, 0, 0);
	int fpsRet = fps.Calculate();
	if (fpsRet != -1) {
		std::ostringstream msg;
		msg << "FPS: " << fpsRet;
		fpsTextLine.SetText(renderer, font, msg.str(), {255, 255, 255, 255});
	}
}

//-------------------------
//Event handlers
//-------------------------

void World::QuitEvent() {
	//two-step logout
	SendDisconnectRequest();
	SetSceneSignal(SceneSignal::QUIT);
}

void World::MouseMotion(SDL_MouseMotionEvent const& event) {
	disconnectButton.MouseMotion(event);
	shutdownButton.MouseMotion(event);
}

void World::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	disconnectButton.MouseButtonDown(event);
	shutdownButton.MouseButtonDown(event);
}

void World::MouseButtonUp(SDL_MouseButtonEvent const& event) {
	if (disconnectButton.MouseButtonUp(event) == Button::State::RELEASED) {
		SendLogoutRequest();
	}
	if (shutdownButton.MouseButtonUp(event) == Button::State::RELEASED) {
		SendAdminShutdownRequest();
	}
}

void World::MouseWheel(SDL_MouseWheelEvent const& event) {
	//
}

void World::KeyDown(SDL_KeyboardEvent const& event) {
	//BUGFIX: SDL2 introduced key repeats, so I need to ignore it
	if (event.repeat) {
		return;
	}

	//hotkeys
	switch(event.keysym.sym) {
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
	switch(event.keysym.sym) {
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

void World::KeyUp(SDL_KeyboardEvent const& event) {
	//BUGFIX: SDL2 introduced key repeats, so I need to ignore it
	if (event.repeat) {
		return;
	}

	//character movement
	if (!localCharacter) {
		return;
	}
	Vector2 motion = localCharacter->GetMotion();
	switch(event.keysym.sym) {
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