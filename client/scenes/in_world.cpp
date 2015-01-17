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
		SendLocalCharacterMotion();
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
	std::ostringstream msg;
	msg << fps.GetFrameRate();
	font.DrawStringTo(msg.str(), screen, 0, 0);
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
	SendLocalCharacterMotion();
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
	SendLocalCharacterMotion();
}

//-------------------------
//Basic connections
//-------------------------

void InWorld::HandlePacket(SerialPacket* const argPacket) {
	switch(argPacket->type) {
		//heartbeat system
		case SerialPacketType::PING:
			HandlePing(static_cast<ServerPacket*>(argPacket));
		break;
		case SerialPacketType::PONG:
			HandlePong(static_cast<ServerPacket*>(argPacket));
		break;

		//game server connections
		case SerialPacketType::LOGOUT_RESPONSE:
			HandleLogoutResponse(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::DISCONNECT_RESPONSE:
			HandleDisconnectResponse(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::DISCONNECT_FORCED:
			HandleDisconnectForced(static_cast<ClientPacket*>(argPacket));
		break;

		//map management
		case SerialPacketType::REGION_CONTENT:
			HandleRegionContent(static_cast<RegionPacket*>(argPacket));
		break;

		//character management
		case SerialPacketType::CHARACTER_CREATE:
			HandleCharacterCreate(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_DELETE:
			HandleCharacterDelete(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_CHARACTER_EXISTS:
			HandleCharacterQueryExists(static_cast<CharacterPacket*>(argPacket));
		break;

		//character movement
		case SerialPacketType::CHARACTER_SET_ROOM:
			HandleCharacterSetRoom(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_SET_ORIGIN:
			HandleCharacterSetOrigin(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_SET_MOTION:
			HandleCharacterSetMotion(static_cast<CharacterPacket*>(argPacket));
		break;

		//rejection messages
		case SerialPacketType::REGION_REJECTION:
		case SerialPacketType::CHARACTER_REJECTION:
			throw(terminal_error(static_cast<TextPacket*>(argPacket)->text));
		break;
		case SerialPacketType::SHUTDOWN_REJECTION:
			throw(std::runtime_error(static_cast<TextPacket*>(argPacket)->text));
		break;

		//errors
		default: {
			std::ostringstream msg;
			msg << "Unknown SerialPacketType encountered in InWorld: " << static_cast<int>(argPacket->type);
			throw(std::runtime_error(msg.str()));
		}
		break;
	}
}

void InWorld::HandlePing(ServerPacket* const argPacket) {
	ServerPacket newPacket;
	newPacket.type = SerialPacketType::PONG;
	network.SendTo(argPacket->srcAddress, &newPacket);
}

void InWorld::HandlePong(ServerPacket* const argPacket) {
	if (*network.GetIPAddress(Channels::SERVER) != argPacket->srcAddress) {
		throw(std::runtime_error("Heartbeat message received from an unknown source"));
	}
	attemptedBeats = 0;
	lastBeat = Clock::now();
}

//-------------------------
//Connection control
//-------------------------

void InWorld::SendLogoutRequest() {
	ClientPacket newPacket;

	//send a logout request
	newPacket.type = SerialPacketType::LOGOUT_REQUEST;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::SendDisconnectRequest() {
	ClientPacket newPacket;

	//send a disconnect request
	newPacket.type = SerialPacketType::DISCONNECT_REQUEST;
	newPacket.clientIndex = clientIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::SendShutdownRequest() {
	ClientPacket newPacket;

	//send a shutdown request
	newPacket.type = SerialPacketType::SHUTDOWN_REQUEST;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::HandleLogoutResponse(ClientPacket* const argPacket) {
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

void InWorld::HandleDisconnectResponse(ClientPacket* const argPacket) {
	HandleLogoutResponse(argPacket);//shortcut
	SetNextScene(SceneList::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have successfully logged out";
}

void InWorld::HandleDisconnectForced(ClientPacket* const argPacket) {
	HandleDisconnectResponse(argPacket);//shortcut
	SetNextScene(SceneList::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have been forcibly disconnected by the server";
}

void InWorld::CheckHeartBeat() {
	//check the connection (heartbeat)
	if (Clock::now() - lastBeat > std::chrono::seconds(3)) {
		if (attemptedBeats > 2) {
			//escape to the disconnect screen
			SendDisconnectRequest();
			SetNextScene(SceneList::DISCONNECTEDSCREEN);
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
//map management
//-------------------------

void InWorld::SendRegionRequest(int roomIndex, int x, int y) {
	RegionPacket packet;

	//pack the region's data
	packet.type = SerialPacketType::REGION_REQUEST;
	packet.roomIndex = roomIndex;
	packet.x = x;
	packet.y = y;

	network.SendTo(Channels::SERVER, &packet);
}

void InWorld::HandleRegionContent(RegionPacket* const argPacket) {
	//replace existing regions
	regionPager.UnloadIf([&](Region const& region) -> bool {
		return region.GetX() == argPacket->x && region.GetY() == argPacket->y;
	});
	regionPager.PushRegion(argPacket->region);

	//clean up after the serial code
	delete argPacket->region;
	argPacket->region = nullptr;
}

void InWorld::UpdateMap() {
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
			if (!regionPager.FindRegion(i, j)) {
				SendRegionRequest(roomIndex, i, j);
			}
		}
	}
}

//-------------------------
//entity management
//-------------------------

//DOCS: preexisting characters will result in query responses
//DOCS: new characters will result in create messages
//DOCS: this client's character will exist in both (skipped)

void InWorld::HandleCharacterCreate(CharacterPacket* const argPacket) {
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
	character->SetOrigin(argPacket->origin);
	character->SetMotion(argPacket->motion);
	character->SetBounds({CHARACTER_BOUNDS_X, CHARACTER_BOUNDS_Y, CHARACTER_BOUNDS_WIDTH, CHARACTER_BOUNDS_HEIGHT});
	character->SetHandle(argPacket->handle);
	character->SetAvatar(argPacket->avatar);
	character->SetOwner(argPacket->accountIndex);
	character->CorrectSprite();

	//check for this player's character
	if (character->GetOwner() == accountIndex) {
		localCharacter = static_cast<LocalCharacter*>(character);

		//focus the camera on this character
		camera.marginX = (camera.width / 2 - localCharacter->GetSprite()->GetImage()->GetClipW() / 2);
		camera.marginY = (camera.height/ 2 - localCharacter->GetSprite()->GetImage()->GetClipH() / 2);

		//focus on this character's info
		characterIndex = argPacket->characterIndex;
		roomIndex = argPacket->roomIndex;
	}

	//debug
	std::cout << "Create, total: " << characterMap.size() << std::endl;
}

void InWorld::HandleCharacterDelete(CharacterPacket* const argPacket) {
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

		//clear the room
		roomIndex = -1;
	}

	//remove this character
	characterMap.erase(characterIt);

	//debug
	std::cout << "Delete, total: " << characterMap.size() << std::endl;
}

void InWorld::HandleCharacterQueryExists(CharacterPacket* const argPacket) {
	//prevent a double message about this player's character
	if (argPacket->accountIndex == accountIndex) {
		return;
	}

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
	character->SetAvatar(argPacket->avatar);
	character->SetOwner(argPacket->accountIndex);
	character->CorrectSprite();

	//debug
	std::cout << "Query, total: " << characterMap.size() << std::endl;
}

void InWorld::HandleCharacterSetRoom(CharacterPacket* const argPacket) {
	//someone else's character
	if (argPacket->characterIndex != characterIndex) {
		characterMap.erase(argPacket->characterIndex);
		return;
	}

	//this character is moving between rooms
	roomIndex = argPacket->roomIndex;

	//set the character's info
	localCharacter->SetOrigin(argPacket->origin);
	localCharacter->SetMotion(argPacket->motion);
	localCharacter->CorrectSprite();

	//clear the old room's data
	regionPager.UnloadAll();
	monsterMap.clear();

	//use the jenky pattern for std::map to skip this player's character
	for (std::map<int, BaseCharacter>::iterator it = characterMap.begin(); it != characterMap.end(); /* EMPTY */ ) {
		if (it->first != characterIndex) {
			it = characterMap.erase(it);
		}
		else {
			++it;
		}
	}

	//request the info on characters in this room
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::QUERY_CHARACTER_EXISTS;
	newPacket.roomIndex = roomIndex;
	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::HandleCharacterSetOrigin(CharacterPacket* const argPacket) {
	//TODO: Authentication
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

void InWorld::HandleCharacterSetMotion(CharacterPacket* const argPacket) {
	//TODO: Authentication
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
//player movement
//-------------------------

//TODO: add a "movement" packet type
void InWorld::SendLocalCharacterMotion() {
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_SET_MOTION;

	newPacket.accountIndex = accountIndex;
	newPacket.characterIndex = characterIndex;
	newPacket.roomIndex = roomIndex;
	newPacket.origin = localCharacter->GetOrigin();
	newPacket.motion = localCharacter->GetMotion();

	network.SendTo(Channels::SERVER, &newPacket);
}

std::list<BoundingBox> InWorld::GenerateCollisionGrid(Entity* ptr, int tileWidth, int tileHeight) {
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
			//check to see if this tile is solid
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