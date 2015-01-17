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
#include "in_world.hpp"

#include "channels.hpp"
#include "utility.hpp"

#include "terminal_error.hpp"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

//-------------------------
//these should've come standard
//-------------------------

bool operator==(IPaddress lhs, IPaddress rhs) {
	return lhs.host == rhs.host && lhs.port == rhs.port;
}

bool operator!=(IPaddress lhs, IPaddress rhs) {
	return !(lhs == rhs);
}

//-------------------------
//Public access members
//-------------------------

InWorld::InWorld(int* const argClientIndex,	int* const argAccountIndex):
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
	//TODO: add the tilesheet to the map system
	//TODO: Tile size and tile sheet should be loaded elsewhere
	tileSheet.Load(config["dir.tilesets"] + "overworld.bmp", 32, 32);

	//Send the character data
	//TODO: login scene, prompt, etc.
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_LOAD;
	strncpy(newPacket.handle, config["client.handle"].c_str(), PACKET_STRING_SIZE);
	strncpy(newPacket.avatar, config["client.avatar"].c_str(), PACKET_STRING_SIZE);
	newPacket.accountIndex = accountIndex;
	network.SendTo(Channels::SERVER, &newPacket);

	//query the world state
	memset(&newPacket, 0, MAX_PACKET_SIZE);
	newPacket.type = SerialPacketType::QUERY_CHARACTER_EXISTS;
	network.SendTo(Channels::SERVER, &newPacket);

	//set the camera's values
	camera.width = GetScreen()->w;
	camera.height = GetScreen()->h;

	//debug
	//
}

InWorld::~InWorld() {
	//unload the local data
	characterMap.clear();
	monsterMap.clear();
}

//-------------------------
//Frame loop
//-------------------------

void InWorld::FrameStart() {
	//
}

void InWorld::Update() {
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

void InWorld::FrameEnd() {
	//
}

void InWorld::RenderFrame() {
	SDL_FillRect(GetScreen(), 0, 0);
	Render(GetScreen());
	SDL_Flip(GetScreen());
	fps.Calculate();
}

void InWorld::Render(SDL_Surface* const screen) {
	//draw the map
	for (std::list<Region>::iterator it = regionPager.GetContainer()->begin(); it != regionPager.GetContainer()->end(); it++) {
		tileSheet.DrawRegionTo(screen, &(*it), camera.x, camera.y);
	}

	//draw the entities
	for (auto& it : characterMap) {
		//TODO: depth ordering
		it.second.DrawTo(screen, camera.x, camera.y);
	}
	for (auto& it : monsterMap) {
		//TODO: depth ordering
		it.second.DrawTo(screen, camera.x, camera.y);
	}

	//draw UI
	disconnectButton.DrawTo(screen);
	shutDownButton.DrawTo(screen);
	font.DrawStringTo(to_string_custom(fps.GetFrameRate()), screen, 0, 0);
}

//-------------------------
//Event handlers
//-------------------------

void InWorld::QuitEvent() {
	//two-step logout
	SendDisconnectRequest();
	SetNextScene(SceneList::QUIT);
}

void InWorld::MouseMotion(SDL_MouseMotionEvent const& motion) {
	disconnectButton.MouseMotion(motion);
	shutDownButton.MouseMotion(motion);
}

void InWorld::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	disconnectButton.MouseButtonDown(button);
	shutDownButton.MouseButtonDown(button);
}

void InWorld::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (disconnectButton.MouseButtonUp(button) == Button::State::HOVER && button.button == SDL_BUTTON_LEFT) {
		SendLogoutRequest();
	}
	if (shutDownButton.MouseButtonUp(button) == Button::State::HOVER && button.button == SDL_BUTTON_LEFT) {
		SendShutdownRequest();
	}
}

void InWorld::KeyDown(SDL_KeyboardEvent const& key) {
	//hotkeys
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			//TODO: the escape key should actually control menus and stuff
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

void InWorld::KeyUp(SDL_KeyboardEvent const& key) {
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