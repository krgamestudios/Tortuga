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

	//debug
	//
}

InWorld::~InWorld() {
	//
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

	//update the map
	UpdateMap();

	//check the connection (heartbeat)
	if (Clock::now() - lastBeat > std::chrono::seconds(3)) {
		if (attemptedBeats > 2) {
			//two-step logout
			SendLogoutRequest();
			SendDisconnectRequest();

			SetNextScene(SceneList::DISCONNECTEDSCREEN);
			ConfigUtility::GetSingleton()["client.disconnectMessage"] = "Error: Lost connection to the server";
		}

		ServerPacket newPacket;
		newPacket.type = SerialPacketType::PING;
		network.SendTo(Channels::SERVER, &newPacket);

		attemptedBeats++;
		lastBeat = Clock::now();
	}
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
	SendLogoutRequest();
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
			//the escape key should actually control menus and stuff
			SendLogoutRequest();
		break;
	}
}

void InWorld::KeyUp(SDL_KeyboardEvent const& key) {
	//
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
	accountIndex = -1;

	//TODO: unload the character

	SendDisconnectRequest();
}

void InWorld::HandleDisconnectResponse(ClientPacket* const argPacket) {
	SetNextScene(SceneList::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have successfully logged out";
}

void InWorld::HandleDisconnectForced(ClientPacket* const argPacket) {
	//TODO: More needed in the disconnection
	SetNextScene(SceneList::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have been forcibly disconnected by the server";
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
	regionPager.UnloadRegion(argPacket->x, argPacket->y);
	regionPager.PushRegion(argPacket->region);

	//clean up after the serial code
	delete argPacket->region;
	argPacket->region = nullptr;
}

void InWorld::UpdateMap() {
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
				SendRegionRequest(0, i, j);
			}
		}
	}
}

void InWorld::HandleCharacterCreate(CharacterPacket* const argPacket) {
	//TODO: HandleCharacterCreate()
	std::cout << "HandleCharacterCreate" << std::endl;
}

void InWorld::HandleCharacterDelete(CharacterPacket* const argPacket) {
	//TODO: HandleCharacterDelete()
	std::cout << "HandleCharacterDelete" << std::endl;
}

void InWorld::HandleCharacterQueryExists(CharacterPacket* const argPacket) {
	//TODO: HandleCharacterQueryExists()
	std::cout << "HandleCharacterQueryExists" << std::endl;
	//NOTE: preexisting characters will result in query responses
	//NOTE: new characters will result in create messages
	//NOTE: this client's character will exist in both
}