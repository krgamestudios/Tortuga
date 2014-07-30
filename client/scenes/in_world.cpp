/* Copyright: (c) Kayne Ruse 2013, 2014
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

#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <iostream>

//-------------------------
//Public access members
//-------------------------

InWorld::InWorld(
	ConfigUtility* const argConfig,
	UDPNetworkUtility* const argNetwork,
	int* const argClientIndex,
	int* const argAccountIndex,
	int* const argCharacterIndex,
	CharacterMap* argCharacterMap
	):
	config(*argConfig),
	network(*argNetwork),
	clientIndex(*argClientIndex),
	accountIndex(*argAccountIndex),
	characterIndex(*argCharacterIndex),
	characterMap(*argCharacterMap)
{
	//register the pager
	lua_pushstring(lua, TORTUGA_REGION_PAGER_PSEUDO_INDEX);
	lua_pushlightuserdata(lua, &pager);
	lua_settable(L, LUA_REGISTRYINDEX);

	//register the tilesheet
	lua_pushstring(lua, TORTUGA_TILE_SHEET_PSEUDO_INDEX);
	lua_pushlightuserdata(lua, &tileSheet);
	lua_settable(L, LUA_REGISTRYINDEX);

	//setup the component objecrs
	pager.SetLuaState(lua);

	//get the config info
	lua_getglobal(lua, "config");
	lua_getfield(lua, -1, "dir");
	lua_getfield(lua, -1, "fonts");
	std::string fonts = lua_tostring(lua, -1);
	lua_getfield(lua, -2, "interface");
	std::string interface = lua_tostring(lua, -1);
	lua_getfield(lua, -3, "sprites");
	std::string sprites = lua_tostring(lua, -1);
	lua_getfield(lua, -4, "scripts");
	std::string scripts = lua_tostring(lua, -1);
	lua_pop(lua, 6);

	//run the additional scripts
	if (luaL_dofile(lua, (scripts + "in_world.lua").c_str())) {
		throw(std::runtime_error(std::string() + "Failed to run in_world.lua: " + lua_tostring(lua, -1) ));
	}

	//setup the utility objects
	buttonImage.LoadSurface(interface + "button_menu.bmp");
	buttonImage.SetClipH(buttonImage.GetClipH()/3);
	font.LoadSurface(fonts + "pk_white_8.bmp");

	//pass the utility objects
	backButton.SetImage(&buttonImage);
	backButton.SetFont(&font);

	//set the button positions
	backButton.SetX(50);
	backButton.SetY(50 + buttonImage.GetClipH() * 0);

	//set the button texts
	backButton.SetText("Back");

	//entities
	character.GetSprite()->LoadSurface(sprites + "elliot2.bmp", 4, 4);
	character.SetBoundingBox({0, 0,
		character.GetSprite()->GetImage()->GetClipW(),
		character.GetSprite()->GetImage()->GetClipH()
	});
//-------------------------
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
	//TODO: add the tilesheet to the map system?
	tileSheet.Load(config["dir.tilesets"] + "terrain.bmp", 12, 15);

	//request a sync
	RequestSynchronize();

	//debug
	//
}

InWorld::~InWorld() {
	//unregister the map components
	lua_pushstring(lua, TORTUGA_REGION_PAGER_PSEUDO_INDEX);
	lua_pushstring(lua, TORTUGA_TILE_SHEET_PSEUDO_INDEX);
	lua_pushnil(lua);
	lua_settable(lua, LUA_REGISTRYINDEX);
	lua_pushnil(lua);
	lua_settable(lua, LUA_REGISTRYINDEX);
}

//-------------------------
//Frame loop
//-------------------------

void InWorld::FrameStart() {
	//
}

void InWorld::Update(double delta) {
	//suck in and process all waiting packets
	SerialPacket* packetBuffer = static_cast<SerialPacket*>(malloc(MAX_PACKET_SIZE));
	while(network.Receive(packetBuffer)) {
		HandlePacket(packetBuffer);
	}
	free(static_cast<void*>(packetBuffer));

	//update the characters
	for (auto& it : characterMap) {
		it.second.Update(delta);
	}

	//TODO: Check collisions here
	//check for collisions with the map
	BoundingBox wallBounds = {0, 0, tileSheet.GetTileW(), tileSheet.GetTileH()};
	const int xCount = character.GetBoundingBox().w / wallBounds.w + 1;
	const int yCount = character.GetBoundingBox().h / wallBounds.h + 1;

	for (int i = -1; i <= xCount; ++i) {
		for (int j = -1; j <= yCount; ++j) {
			//set the wall's position
			wallBounds.x = wallBounds.w * i + snapToBase((double)wallBounds.w, character.GetOrigin().x);
			wallBounds.y = wallBounds.h * j + snapToBase((double)wallBounds.h, character.GetOrigin().y);

			if (!pager.GetSolid(wallBounds.x / wallBounds.w, wallBounds.y / wallBounds.h)) {
				continue;
			}

			if ((character.GetOrigin() + character.GetBoundingBox()).CheckOverlap(wallBounds)) {
				character.SetOrigin(character.GetOrigin() - (character.GetMotion() * delta));
				character.SetMotion({0,0});
				character.CorrectSprite();
			}
		}
	}

	//update the camera
	if(localCharacter) {
		camera.x = localCharacter->GetOrigin().x - camera.marginX;
		camera.y = localCharacter->GetOrigin().y - camera.marginY;
	}

	//check the map
	UpdateMap();
}

void InWorld::FrameEnd() {
	//
}

void InWorld::RenderFrame() {
//	SDL_FillRect(GetScreen(), 0, 0);
	Render(GetScreen());
	SDL_Flip(GetScreen());
	fps.Calculate();
}

void InWorld::Render(SDL_Surface* const screen) {
	//draw the map
	for (std::list<Region>::iterator it = regionPager.GetContainer()->begin(); it != regionPager.GetContainer()->end(); it++) {
		tileSheet.DrawRegionTo(screen, &(*it), camera.x, camera.y);
	}

	//draw characters
	for (auto& it : characterMap) {
		//BUG: #29 drawing order according to Y origin
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
	//exit the game AND the server
	RequestDisconnect();
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
	if (disconnectButton.MouseButtonUp(button) == Button::State::HOVER) {
		RequestDisconnect();
	}
	if (shutDownButton.MouseButtonUp(button) == Button::State::HOVER) {
		RequestShutDown();
	}
}

void InWorld::KeyDown(SDL_KeyboardEvent const& key) {
	if (!localCharacter) {
		return;
	}

	//player movement
	Vector2 motion = localCharacter->GetMotion();
	switch(key.keysym.sym) {
		case SDLK_LEFT:
			motion.x -= CHARACTER_WALKING_SPEED;
		break;
		case SDLK_RIGHT:
			motion.x += CHARACTER_WALKING_SPEED;
		break;
		case SDLK_UP:
			motion.y -= CHARACTER_WALKING_SPEED;
		break;
		case SDLK_DOWN:
			motion.y += CHARACTER_WALKING_SPEED;
		break;
		default:
			return;
	}
	localCharacter->SetMotion(motion);
	localCharacter->CorrectSprite();
	SendPlayerUpdate();
}

void InWorld::KeyUp(SDL_KeyboardEvent const& key) {
	if (!localCharacter) {
		return;
	}

	//player movement
	Vector2 motion = localCharacter->GetMotion();
	switch(key.keysym.sym) {
		//NOTE: The use of min/max here are to prevent awkward movements
		case SDLK_LEFT:
			motion.x = std::min(motion.x + CHARACTER_WALKING_SPEED, 0.0);
		break;
		case SDLK_RIGHT:
			motion.x = std::max(motion.x - CHARACTER_WALKING_SPEED, 0.0);
		break;
		case SDLK_UP:
			motion.y = std::min(motion.y + CHARACTER_WALKING_SPEED, 0.0);
		break;
		case SDLK_DOWN:
			motion.y = std::max(motion.y - CHARACTER_WALKING_SPEED, 0.0);
		break;
		default:
			return;
	}
	localCharacter->SetMotion(motion);
	localCharacter->CorrectSprite();
	SendPlayerUpdate();
}

//-------------------------
//Network handlers
//-------------------------

void InWorld::HandlePacket(SerialPacket* const argPacket) {
	switch(argPacket->type) {
		case SerialPacketType::DISCONNECT:
			HandleDisconnect(argPacket);
		break;
		case SerialPacketType::CHARACTER_NEW:
			HandleCharacterNew(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_DELETE:
			HandleCharacterDelete(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_UPDATE:
			HandleCharacterUpdate(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::REGION_CONTENT:
			HandleRegionContent(static_cast<RegionPacket*>(argPacket));
		break;
		//handle errors
		default:
			throw(std::runtime_error(std::string() + "Unknown SerialPacketType encountered in InWorld: " + to_string_custom(static_cast<int>(argPacket->type)) ));
		break;
	}
}

void InWorld::HandleDisconnect(SerialPacket* const argPacket) {
	SetNextScene(SceneList::CLEANUP);
}

void InWorld::HandleCharacterNew(CharacterPacket* const argPacket) {
	if (characterMap.find(argPacket->characterIndex) != characterMap.end()) {
		throw(std::runtime_error("Cannot create duplicate characters"));
	}

	//create the character object
	Character& newCharacter = characterMap[argPacket->characterIndex];

	//fill out the character's members
	newCharacter.SetHandle(argPacket->handle);
	newCharacter.SetAvatar(argPacket->avatar);

	newCharacter.GetSprite()->LoadSurface(config["dir.sprites"] + newCharacter.GetAvatar(), 4, 4);

	newCharacter.SetOrigin(argPacket->origin);
	newCharacter.SetMotion(argPacket->motion);

	(*newCharacter.GetStats()) = argPacket->stats;

	//bookkeeping code
	newCharacter.CorrectSprite();

	//catch this client's player object
	if (argPacket->accountIndex == accountIndex && !localCharacter) {
		characterIndex = argPacket->characterIndex;
		localCharacter = &newCharacter;

		//setup the camera
		//TODO: move this?
		camera.width = GetScreen()->w;
		camera.height = GetScreen()->h;

		//center on the player's character
		camera.marginX = (GetScreen()->w / 2 - localCharacter->GetSprite()->GetImage()->GetClipW() / 2);
		camera.marginY = (GetScreen()->h / 2 - localCharacter->GetSprite()->GetImage()->GetClipH() / 2);
	}
}

void InWorld::HandleCharacterDelete(CharacterPacket* const argPacket) {
	//TODO: authenticate when own character is being deleted (linked to a TODO in the server)

	//catch this client's player object
	if (argPacket->characterIndex == characterIndex) {
		characterIndex = -1;
		localCharacter = nullptr;
	}

	characterMap.erase(argPacket->characterIndex);
}

void InWorld::HandleCharacterUpdate(CharacterPacket* const argPacket) {
	if (characterMap.find(argPacket->characterIndex) == characterMap.end()) {
		std::cout << "Warning: HandleCharacterUpdate() is passing to HandleCharacterNew()" << std::endl;
		HandleCharacterNew(argPacket);
		return;
	}

	Character& character = characterMap[argPacket->characterIndex];

	//other characters moving
	if (argPacket->characterIndex != characterIndex) {
		character.SetOrigin(argPacket->origin);
		character.SetMotion(argPacket->motion);
		character.CorrectSprite();
	}
}

void InWorld::HandleRegionContent(RegionPacket* const argPacket) {
	//replace existing regions
	regionPager.UnloadRegion(argPacket->x, argPacket->y);
	regionPager.PushRegion(argPacket->region);

	//clean up after the serial code
	delete argPacket->region;
	argPacket->region = nullptr;
}

//-------------------------
//Server control
//-------------------------

void InWorld::RequestSynchronize() {
	ClientPacket newPacket;

	//request a sync
	newPacket.type = SerialPacketType::SYNCHRONIZE;
	newPacket.clientIndex = clientIndex;
	newPacket.accountIndex = accountIndex;

	//TODO: location, range for sync request

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::SendPlayerUpdate() {
	CharacterPacket newPacket;

	//pack the packet
	newPacket.type = SerialPacketType::CHARACTER_UPDATE;

	newPacket.characterIndex = characterIndex;
	//NOTE: omitting the handle and avatar here
	newPacket.accountIndex = accountIndex;
	newPacket.roomIndex = 0; //TODO: room index
	newPacket.origin = localCharacter->GetOrigin();
	newPacket.motion = localCharacter->GetMotion();
	newPacket.stats = *localCharacter->GetStats();

	//TODO: gameplay components: equipment, items, buffs, debuffs

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::RequestDisconnect() {
	ClientPacket newPacket;

	//send a disconnect request
	newPacket.type = SerialPacketType::DISCONNECT;
	newPacket.clientIndex = clientIndex;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::RequestShutDown() {
	ClientPacket newPacket;

	//send a shutdown request
	newPacket.type = SerialPacketType::SHUTDOWN;
	newPacket.clientIndex = clientIndex;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::RequestRegion(int roomIndex, int x, int y) {
	RegionPacket packet;

	//pack the region's data
	packet.type = SerialPacketType::REGION_REQUEST;
	packet.roomIndex = roomIndex;
	packet.x = x;
	packet.y = y;

	network.SendTo(Channels::SERVER, &packet);
}

//-------------------------
//Utilities
//-------------------------

//TODO: convert this into a more generic function?; using parameters for the bounds
void InWorld::UpdateMap() {
	//these represent the zone of regions that the client needs loaded, including the mandatory buffers (+1/-1)
	int xStart = snapToBase(REGION_WIDTH, camera.x/tileSheet.GetTileW()) - REGION_WIDTH;
	int xEnd = snapToBase(REGION_WIDTH, (camera.x+camera.width)/tileSheet.GetTileW()) + REGION_WIDTH;

	int yStart = snapToBase(REGION_HEIGHT, camera.y/tileSheet.GetTileH()) - REGION_HEIGHT;
	int yEnd = snapToBase(REGION_HEIGHT, (camera.y+camera.height)/tileSheet.GetTileH()) + REGION_HEIGHT;

	//prune distant regions
	for (std::list<Region>::iterator it = regionPager.GetContainer()->begin(); it != regionPager.GetContainer()->end(); /* EMPTY */) {
		//check if the region is outside of this area
		if (it->GetX() < xStart || it->GetX() > xEnd || it->GetY() < yStart || it->GetY() > yEnd) {

			//clunky, but the alternative was time consuming
			int tmpX = it->GetX();
			int tmpY = it->GetY();
			++it;

			regionPager.UnloadRegion(tmpX, tmpY);
			continue;
		}
		++it;
	}

	//request empty regions within this zone
	for (int i = xStart; i <= xEnd; i += REGION_WIDTH) {
		for (int j = yStart; j <= yEnd; j += REGION_HEIGHT) {
			if (!regionPager.FindRegion(i, j)) {
				RequestRegion(0, i, j);
			}
		}
	}
}
