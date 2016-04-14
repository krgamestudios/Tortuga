/* Copyright: (c) Kayne Ruse 2013-2016
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
#include "culling_defines.hpp"
#include "ip_operators.hpp"
#include "fatal_error.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

//-------------------------
//static functions
//-------------------------

//TODO: (3) proper checksum
static int regionContentCheck(Region const* region) {
	int sum = 0;
	for(int i = 0; i < REGION_WIDTH; i++) {
		for (int j = 0; j < REGION_HEIGHT; j++) {
			for (int k = 0; k < REGION_DEPTH; k++) {
				sum += region->GetTile(i, j, k);
			}
		}
	}
	return sum;
}

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
	std::list<std::string> slotNames = {
			"slot 1 green.png",
			"slot 2 green.png",
			"slot 3 green.png",
			"slot 4 green.png",
			"slot 5 green.png",
			"slot 6 green.png",
			"slot 7 green.png",
			"slot 8 green.png",
			"slot 1 red.png",
			"slot 2 red.png",
			"slot 3 red.png",
			"slot 4 red.png",
			"slot 5 red.png",
			"slot 6 red.png",
			"slot 7 red.png",
			"slot 8 red.png"
		};
	barrierMgr.LoadBaseImage(GetRenderer(), config["dir.sprites"] + "/barrier/base.png");
	barrierMgr.LoadTemplateImages(GetRenderer(), config["dir.sprites"] + "/barrier/", slotNames);

	std::cout << "Templates loaded: " << barrierMgr.GetTemplateContainer()->size() << std::endl;
}

World::~World() {
	//unload the local data
	TTF_CloseFont(font);
	characterMap.clear();
	creatureMap.clear();
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
	catch(fatal_error& e) {
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
	for (auto& it : creatureMap) {
		it.second.Update();
	}

	try {
		//update the map
		UpdateMap();
	}
	catch(fatal_error& e) {
		throw(e);
	}
	catch(std::exception& e) {
		std::cerr << "UpdateMap Error: " << e.what() << std::endl;
	}

	//skip the rest without a local character
	if (!localCharacter) {
		return;
	}

	//TODO: (1) regular query interval
	//cull creatures
	for (std::map<int, BaseCreature>::iterator it = creatureMap.begin(); it != creatureMap.end(); /* */) {
		if ( (localCharacter->GetOrigin() - it->second.GetOrigin()).Length() > INFLUENCE_RADIUS) {
			creatureMap.erase(it++);
		}
		else {
			it++;
		}
	}

	//TODO: cull barriers

	//get the collidable boxes
	std::list<BoundingBox> boxList = GenerateCollisionGrid(localCharacter, tileSheet.GetTileW(), tileSheet.GetTileH());

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
	}

	//draw the entities
	for (auto& it : characterMap) {
		//BUG: #29 Characters (and other entities) are drawn out of order
		it.second.DrawTo(renderer, camera.x, camera.y);
	}
	for (auto& it : creatureMap) {
		it.second.DrawTo(renderer, camera.x, camera.y);
	}
	barrierMgr.DrawTo(renderer, camera.x, camera.y);

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
	//TODO: Sliding against walls controls
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
		case SerialPacketType::CHARACTER_UNLOAD:
			hCharacterUnload(static_cast<CharacterPacket*>(argPacket));
		break;

		case SerialPacketType::QUERY_CHARACTER_EXISTS:
			hQueryCharacterExists(static_cast<CharacterPacket*>(argPacket));
		break;

		case SerialPacketType::CHARACTER_MOVEMENT:
			hCharacterMovement(static_cast<CharacterPacket*>(argPacket));
		break;

		//creature management
		case SerialPacketType::CREATURE_UPDATE:
			hCreatureUpdate(static_cast<CreaturePacket*>(argPacket));
		break;

		case SerialPacketType::CREATURE_CREATE:
			hCreatureCreate(static_cast<CreaturePacket*>(argPacket));
		break;
		case SerialPacketType::CREATURE_UNLOAD:
			hCreatureUnload(static_cast<CreaturePacket*>(argPacket));
		break;

		case SerialPacketType::QUERY_CREATURE_EXISTS:
			hQueryCreatureExists(static_cast<CreaturePacket*>(argPacket));
		break;

		case SerialPacketType::CREATURE_MOVEMENT:
			hCreatureMovement(static_cast<CreaturePacket*>(argPacket));
		break;

		//barrier management
		case SerialPacketType::BARRIER_UPDATE:
			hBarrierUpdate(static_cast<BarrierPacket*>(argPacket));
		break;

		case SerialPacketType::BARRIER_CREATE:
			hBarrierCreate(static_cast<BarrierPacket*>(argPacket));
		break;
		case SerialPacketType::BARRIER_UNLOAD:
			hBarrierUnload(static_cast<BarrierPacket*>(argPacket));
		break;

		case SerialPacketType::QUERY_BARRIER_EXISTS:
			hQueryBarrierExists(static_cast<BarrierPacket*>(argPacket));
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
			throw(fatal_error(static_cast<TextPacket*>(argPacket)->text));
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

//-------------------------
//heartbeat system
//-------------------------

void World::hPing(ServerPacket* const argPacket) {
	ServerPacket newPacket;
	newPacket.type = SerialPacketType::PONG;
	network.SendTo(argPacket->srcAddress, &newPacket);
}

void World::hPong(ServerPacket* const argPacket) {
	if (*network.GetIPAddress(Channels::SERVER) != argPacket->srcAddress) {
		throw(std::runtime_error("Heartbeat message received from an unknown source"));
	}
	attemptedBeats = 0;
	lastBeat = Clock::now();
}

void World::CheckHeartBeat() {
	//check the connection (heartbeat)
	if (Clock::now() - lastBeat > std::chrono::seconds(3)) {
		if (attemptedBeats > 2) {
			//escape to the disconnect screen
			SendDisconnectRequest();
			SetSceneSignal(SceneSignal::DISCONNECTEDSCREEN);
			ConfigUtility::GetSingleton()["client.disconnectMessage"] = "Error: Lost connection to the server";
		}
		else {
			ServerPacket newPacket;
			newPacket.type = SerialPacketType::PING;
			network.SendTo(Channels::SERVER, &newPacket);

			attemptedBeats++;
			lastBeat = Clock::now();
		}
	}
}

//-------------------------
//Connection control
//-------------------------

void World::SendLogoutRequest() {
	ClientPacket newPacket;

	//send a logout request
	newPacket.type = SerialPacketType::LOGOUT_REQUEST;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void World::SendDisconnectRequest() {
	ClientPacket newPacket;

	//send a disconnect request
	newPacket.type = SerialPacketType::DISCONNECT_REQUEST;
	newPacket.clientIndex = clientIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void World::SendAdminDisconnectForced() {
	//TODO: (9) World::SendAdminDisconnectForced()
}

void World::SendAdminShutdownRequest() {
	ClientPacket newPacket;

	//send a shutdown request
	newPacket.type = SerialPacketType::ADMIN_SHUTDOWN_REQUEST;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void World::hLogoutResponse(ClientPacket* const argPacket) {
	if (localCharacter) {
		characterMap.erase(characterIndex);
		localCharacter = nullptr;
	}

	accountIndex = -1;
	characterIndex = -1;

	//reset the camera
	camera.marginX = camera.marginY = 0;

	//because, why not? I guess...
	SendDisconnectRequest();
}

void World::hDisconnectResponse(ClientPacket* const argPacket) {
	hLogoutResponse(argPacket);//shortcut
	SetSceneSignal(SceneSignal::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have successfully logged out";
}

void World::hAdminDisconnectForced(ClientPacket* const argPacket) {
	hDisconnectResponse(argPacket);//shortcut
	SetSceneSignal(SceneSignal::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have been forcibly disconnected by the server";
}

//-------------------------
//map management
//-------------------------

void World::SendRegionRequest(int roomIndex, int x, int y) {
	RegionPacket packet;

	//pack the region's data
	packet.type = SerialPacketType::REGION_REQUEST;
	packet.roomIndex = roomIndex;
	packet.x = x;
	packet.y = y;

	network.SendTo(Channels::SERVER, &packet);
}

void World::hRegionContent(RegionPacket* const argPacket) {
	//replace existing regions
	regionPager.UnloadIf([&](Region const& region) -> bool {
		if (region.GetX() == argPacket->x && region.GetY() == argPacket->y) {
			std::cout << "Region Overwrite: " << region.GetX() << ", " << region.GetY();
			std::cout << "\t" << regionContentCheck(&region) << "\t" << regionContentCheck(argPacket->region) << std::endl;
			return true;
		}
		else {
			return false;
		}
	});
	regionPager.PushRegion(argPacket->region);

	//clean up after the serial code
	delete argPacket->region;
	argPacket->region = nullptr;
}

void World::UpdateMap() {
	if (roomIndex == -1) {
		return;
	}

	//these represent the zone of regions that the client needs loaded, including the mandatory buffers (+1/-1)
	int xStart = snapToBase(REGION_WIDTH, camera.x/tileSheet.GetTileW()) - REGION_WIDTH;
	int xEnd = snapToBase(REGION_WIDTH, (camera.x+camera.width)/tileSheet.GetTileW()) + REGION_WIDTH;

	int yStart = snapToBase(REGION_HEIGHT, camera.y/tileSheet.GetTileH()) - REGION_HEIGHT;
	int yEnd = snapToBase(REGION_HEIGHT, (camera.y+camera.height)/tileSheet.GetTileH()) + REGION_HEIGHT;

	//prune distant regions
	regionPager.GetContainer()->remove_if([&](Region const& region) -> bool {
		return region.GetX() < xStart || region.GetX() > xEnd || region.GetY() < yStart || region.GetY() > yEnd;
	});

	//request empty regions within this zone
	for (int i = xStart; i <= xEnd; i += REGION_WIDTH) {
		for (int j = yStart; j <= yEnd; j += REGION_HEIGHT) {
			Region* region = regionPager.FindRegion(i, j);
			if (!region) {
				//request absent region
				SendRegionRequest(roomIndex, i, j);
			}
			else if (regionContentCheck(region) == 0) {
				//checksum failed
				regionPager.UnloadIf([region](Region const& ref) -> bool {
					//remove the erroneous region
					return region == &ref;
				});
				SendRegionRequest(roomIndex, i, j);
				std::ostringstream msg;
				msg << "Existing region blank: " << roomIndex << ", " << i << ", " << j;
				throw(std::runtime_error(msg.str()));
			}
		}
	}
}

//-------------------------
//character management
//-------------------------

//DOCS: preexisting characters will result in query responses
//DOCS: new characters will result in create messages
//DOCS: this client's character will exist in both (skipped)

void World::hCharacterUpdate(CharacterPacket* const argPacket) {
	//TODO: (1) Authentication
	//NOTE: applies to the local character too

	//check that this character exists
	std::map<int, BaseCharacter>::iterator characterIt = characterMap.find(argPacket->characterIndex);
	if (characterIt != characterMap.end()) {
		//update the origin and motion, if there's a difference
		if (characterIt->second.GetOrigin() != argPacket->origin) {
			characterIt->second.SetOrigin(argPacket->origin);
		}
		if (characterIt->second.GetMotion() != argPacket->motion) {
			characterIt->second.SetMotion(argPacket->motion);
			characterIt->second.CorrectSprite(); //only correct the sprite if the motion changes
		}
	}
}

void World::hCharacterCreate(CharacterPacket* const argPacket) {
	//prevent double message
	if (characterMap.find(argPacket->characterIndex) != characterMap.end()) {
		std::ostringstream msg;
		msg << "Double character creation event; ";
		msg << "Index: " << argPacket->characterIndex << "; ";
		msg << "Handle: " << argPacket->handle;
		throw(std::runtime_error(msg.str()));
	}

	//implicity create and retrieve the entity
	BaseCharacter* character = &characterMap[argPacket->characterIndex];

	//fill the character's info
	character->SetHandle(argPacket->handle);
	character->SetAvatar(GetRenderer(), argPacket->avatar);
	character->SetOwner(argPacket->accountIndex);
	character->SetOrigin(argPacket->origin);
	character->SetMotion(argPacket->motion);
	character->SetBounds(argPacket->bounds);

	character->CorrectSprite();

	//check for this player's character
	if (character->GetOwner() == accountIndex) {
		localCharacter = static_cast<LocalCharacter*>(character);

		//focus the camera on this character's sprite
		camera.marginX = (camera.width / 2 - localCharacter->GetSprite()->GetClipW() / 2);
		camera.marginY = (camera.height/ 2 - localCharacter->GetSprite()->GetClipH() / 2);

		//focus on this character's info
		characterIndex = argPacket->characterIndex;
		roomIndex = argPacket->roomIndex;

		//query the world state (room)
		CharacterPacket characterPacket;
		memset(&characterPacket, 0, MAX_PACKET_SIZE);
		characterPacket.type = SerialPacketType::QUERY_CHARACTER_EXISTS;
		characterPacket.roomIndex = roomIndex;
		characterPacket.origin = localCharacter->GetOrigin();
		network.SendTo(Channels::SERVER, &characterPacket);

		CreaturePacket creaturePacket;
		creaturePacket.type = SerialPacketType::QUERY_CREATURE_EXISTS;
		creaturePacket.roomIndex = roomIndex;
		creaturePacket.origin = localCharacter->GetOrigin();
		network.SendTo(Channels::SERVER, &creaturePacket);

		BarrierPacket barrierPacket;
		barrierPacket.type = SerialPacketType::QUERY_BARRIER_EXISTS;
		barrierPacket.roomIndex = roomIndex;
		barrierPacket.origin = localCharacter->GetOrigin();
		network.SendTo(Channels::SERVER, &barrierPacket);
	}

	//debug
	std::cout << "Character Create, total: " << characterMap.size() << std::endl;
}

void World::hCharacterUnload(CharacterPacket* const argPacket) {
	//ignore if this character doesn't exist
	std::map<int, BaseCharacter>::iterator characterIt = characterMap.find(argPacket->characterIndex);
	if (characterIt == characterMap.end()) {
		return;
	}

	//check for this player's character
	if ((*characterIt).second.GetOwner() == accountIndex) {
		localCharacter = nullptr;

		//clear the camera
		camera.marginX = 0;
		camera.marginY = 0;

		//clear/reset the room
		roomIndex = -1;
		regionPager.UnloadAll();
		barrierMgr.UnloadAll();
		characterMap.clear();
		creatureMap.clear();
	}
	else {
		//remove this character
		characterMap.erase(characterIt);
	}

	//debug
	std::cout << "Character Unload, total: " << characterMap.size() << std::endl;
}

void World::hQueryCharacterExists(CharacterPacket* const argPacket) {
	//prevent a double message about this player's character
	//TODO: why is this commented out?
//	if (argPacket->accountIndex == accountIndex) {
//		return;
//	}

	//ignore characters in a different room (sub-optimal)
	if (argPacket->roomIndex != roomIndex) {
		return;
	}

	//implicitly construct the character if it doesn't exist
	BaseCharacter* character = &characterMap[argPacket->characterIndex];

	//set/update the character's info
	character->SetOrigin(argPacket->origin);
	character->SetMotion(argPacket->motion);
	character->SetBounds({CHARACTER_BOUNDS_X, CHARACTER_BOUNDS_Y, CHARACTER_BOUNDS_WIDTH, CHARACTER_BOUNDS_HEIGHT});
	character->SetHandle(argPacket->handle);
	character->SetAvatar(GetRenderer(), argPacket->avatar);
	character->SetOwner(argPacket->accountIndex);
	character->CorrectSprite();

	//debug
	std::cout << "Character Query, total: " << characterMap.size() << std::endl;
}

void World::hCharacterMovement(CharacterPacket* const argPacket) {
	//TODO: (1) Authentication
	if (argPacket->characterIndex == characterIndex) {
		return;
	}

	//check that this character exists
	std::map<int, BaseCharacter>::iterator characterIt = characterMap.find(argPacket->characterIndex);
	if (characterIt != characterMap.end()) {
		//set the origin and motion
		characterIt->second.SetOrigin(argPacket->origin);
		characterIt->second.SetMotion(argPacket->motion);
		characterIt->second.CorrectSprite();
	}
}

//-------------------------
//creature management
//-------------------------

void World::hCreatureUpdate(CreaturePacket* const argPacket) {
	//BUGFIX: Sometimes crash on exit
	if (!localCharacter) {
		return;
	}

	//Cull creatures that are too far away
	if ( (localCharacter->GetOrigin() - argPacket->origin).Length() > INFLUENCE_RADIUS) {
		//ignore beyond 1000 units
		return;
	}

	//check if this creature exists
	std::map<int, BaseCreature>::iterator creatureIt = creatureMap.find(argPacket->creatureIndex);
	if (creatureIt != creatureMap.end()) {
		//update the origin and motion, if there's a difference
		if (creatureIt->second.GetOrigin() != argPacket->origin) {
			creatureIt->second.SetOrigin(argPacket->origin);
		}
		if (creatureIt->second.GetMotion() != argPacket->motion) {
			creatureIt->second.SetMotion(argPacket->motion);
			creatureIt->second.CorrectSprite(); //only correct the sprite if the motion changes
		}
	}
	else {
		hCreatureCreate(argPacket);
	}
}

void World::hCreatureCreate(CreaturePacket* const argPacket) {
	//check for logic errors
	if (creatureMap.find(argPacket->creatureIndex) != creatureMap.end()) {
		std::ostringstream msg;
		msg << "Double creature creation event; ";
		msg << "Index: " << argPacket->creatureIndex << "; ";
		msg << "Handle: " << argPacket->handle;
		throw(std::runtime_error(msg.str()));
	}

	//ignore creatures from other rooms
	if (roomIndex != argPacket->roomIndex) {
		//temporary error checking
		std::ostringstream msg;
		msg << "Creature from the wrong room received: ";
		msg << "creatureIndex: " << argPacket->creatureIndex << ", roomIndex: " << argPacket->roomIndex;
		throw(std::runtime_error(msg.str()));
	}

	//implicitly create the element
	BaseCreature* creature = &creatureMap[argPacket->creatureIndex];

	//fill the creature's info
	creature->SetHandle(argPacket->handle);
	creature->SetAvatar(GetRenderer(), argPacket->avatar);
	creature->SetBounds(argPacket->bounds);
	creature->SetOrigin(argPacket->origin);
	creature->SetMotion(argPacket->motion);

	//debug
	std::cout << "Creature Create, total: " << creatureMap.size() << std::endl;
}

void World::hCreatureUnload(CreaturePacket* const argPacket) {
	//ignore if this creature doesn't exist
	std::map<int, BaseCreature>::iterator creatureIt = creatureMap.find(argPacket->creatureIndex);
	if (creatureIt == creatureMap.end()) {
		return;
	}

	//remove this creature
	creatureMap.erase(creatureIt);

	//debug
	std::cout << "Creature Unload, total: " << creatureMap.size() << std::endl;
}

void World::hQueryCreatureExists(CreaturePacket* const argPacket) {
	std::cout << "Creature Query" << std::endl;

	//ignore creatures in a different room (sub-optimal)
	if (argPacket->roomIndex != roomIndex) {
		return;
	}

	//implicitly create the element
	BaseCreature* creature = &creatureMap[argPacket->creatureIndex];

	//fill the creature's info
	creature->SetHandle(argPacket->handle);
	creature->SetAvatar(GetRenderer(), argPacket->avatar);
	creature->SetBounds(argPacket->bounds);
	creature->SetOrigin(argPacket->origin);
	creature->SetMotion(argPacket->motion);

	//debug
	std::cout << "Creature Query, total: " << creatureMap.size() << std::endl;
}

void World::hCreatureMovement(CreaturePacket* const argPacket) {
	std::cout << "hCreatureMovement" << std::endl;

	//ignore if this creature doesn't exist
	std::map<int, BaseCreature>::iterator creatureIt = creatureMap.find(argPacket->creatureIndex);
	if (creatureIt == creatureMap.end()) {
		return;
	}

	creatureIt->second.SetOrigin(argPacket->origin);
	creatureIt->second.SetMotion(argPacket->motion);
}

//-------------------------
//barrier management
//-------------------------

void World::hBarrierUpdate(BarrierPacket* const argPacket) {
	//BUGFIX: Sometimes crash on exit
	if (!localCharacter) {
		return;
	}

	//Cull barriers that are too far away
	if ( (localCharacter->GetOrigin() - argPacket->origin).Length() > INFLUENCE_RADIUS) {
		//ignore beyond 1000 units
		return;
	}

	//check if this barrier exists
	BaseBarrier* barrier = barrierMgr.Find(argPacket->barrierIndex);

	if (!barrier) {
		hBarrierCreate(argPacket);
		return;
	}

	//update the origin and motion, if there's a difference
	if (barrier->GetOrigin() != argPacket->origin) {
		barrier->SetOrigin(argPacket->origin);
	}
	barrier->SetStatusArray(argPacket->status);
	barrier->CorrectSprite();
}

void World::hBarrierCreate(BarrierPacket* const argPacket) {
	//check for logic errors

	//ignore barriers from other rooms
	if (roomIndex != argPacket->roomIndex) {
		//temporary error checking
		std::ostringstream msg;
		msg << "Barrier from the wrong room received: ";
		msg << "barrierIndex: " << argPacket->barrierIndex << ", roomIndex: " << argPacket->roomIndex;
		throw(std::runtime_error(msg.str()));
	}

	BaseBarrier* barrier = barrierMgr.Find(argPacket->barrierIndex);

	if (barrier) {
		std::ostringstream msg;
		msg << "Double barrier creation event; ";
		msg << "Index: " << argPacket->barrierIndex;
		throw(std::runtime_error(msg.str()));
	}

	barrier = barrierMgr.Create(argPacket->barrierIndex);

	//fill the barrier's info
	barrier->SetBounds(argPacket->bounds);
	barrier->SetOrigin(argPacket->origin);
	barrier->SetStatusArray(argPacket->status);
	barrier->CorrectSprite();

	//debug
	std::cout << "Barrier Create, total: " << barrierMgr.Size() << std::endl;
}

void World::hBarrierUnload(BarrierPacket* const argPacket) {
	//ignore if this barrier doesn't exist
	barrierMgr.Unload(argPacket->barrierIndex);

	//debug
	std::cout << "Barrier Unload, total: " << barrierMgr.Size() << std::endl;
}

void World::hQueryBarrierExists(BarrierPacket* const argPacket) {
	std::cout << "Barrier Query" << std::endl;

	//ignore barriers in a different room (sub-optimal)
	if (argPacket->roomIndex != roomIndex) {
		return;
	}

	//implicitly create the element
	BaseBarrier* barrier = barrierMgr.Find(argPacket->barrierIndex);

	if (!barrier) {
		barrier = barrierMgr.Create(argPacket->barrierIndex);
	}

	//fill the barrier's info
	barrier->SetBounds(argPacket->bounds);
	barrier->SetOrigin(argPacket->origin);
	barrier->SetStatusArray(argPacket->status);
	barrier->CorrectSprite();

	//debug
	std::cout << "Barrier Query, total: " << barrierMgr.Size() << std::endl;
}

//-------------------------
//chat
//-------------------------

void World::hTextBroadcast(TextPacket* const argPacket) {
	//TODO: (9) World::hTextBroadcast()
}

void World::hTextSpeech(TextPacket* const argPacket) {
	//TODO: (9) World::hTextSpeech()
}

void World::hTextWhisper(TextPacket* const argPacket) {
	//TODO: (9) World::hTextWhisper()
}

//-------------------------
//player movement & collision
//-------------------------

void World::SendLocalCharacterMovement() {
	//BUGFIX: Sometimes crash on exit
	if (!localCharacter) {
		return;
	}

	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_MOVEMENT;

	newPacket.accountIndex = accountIndex;
	newPacket.characterIndex = characterIndex;
	newPacket.roomIndex = roomIndex;
	newPacket.origin = localCharacter->GetOrigin();
	newPacket.motion = localCharacter->GetMotion();

	network.SendTo(Channels::SERVER, &newPacket);
}

std::list<BoundingBox> World::GenerateCollisionGrid(Entity* ptr, int tileWidth, int tileHeight) {
	//prepare for collisions
	BoundingBox wallBounds = {0, 0, tileWidth, tileHeight};
	std::list<BoundingBox> boxList;

	//NOTE: for loops were too dense to work with, so I've just used while loops

	//outer loop
	wallBounds.x = snapToBase((double)wallBounds.w, ptr->GetOrigin().x);
	while(wallBounds.x < (ptr->GetOrigin() + ptr->GetBounds()).x + ptr->GetBounds().w) {
		//inner loop
		wallBounds.y = snapToBase((double)wallBounds.h, ptr->GetOrigin().y);
		while(wallBounds.y < (ptr->GetOrigin() + ptr->GetBounds()).y + ptr->GetBounds().h) {
			//check to see if this tile is solid (non-existant tiles are always false)

			if (regionPager.GetSolid(wallBounds.x / wallBounds.w, wallBounds.y / wallBounds.h)) {
				//push onto the box set
				boxList.push_front(wallBounds);
			}

			//increment
			wallBounds.y += wallBounds.h;
		}

		//increment
		wallBounds.x += wallBounds.w;
	}

	return std::move(boxList);
}