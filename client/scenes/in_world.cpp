/* Copyright: (c) Kayne Ruse 2013
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

#include <algorithm>
#include <cmath>
#include <stdexcept>

//debugging
#include <iostream>
using std::cout;
using std::endl;

//-------------------------
//Public access members
//-------------------------

InWorld::InWorld(ConfigUtility* const argConfig, UDPNetworkUtility* const argNetwork, int* const argClientIndex):
	config(*argConfig),
	network(*argNetwork),
	clientIndex(*argClientIndex)
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
	tileSheet.Load(config["dir.tilesets"] + "terrain.bmp", 12, 15);

	//setup the map object
	mapPager.SetRegionWidth(REGION_WIDTH);
	mapPager.SetRegionHeight(REGION_HEIGHT);
	mapPager.SetRegionDepth(REGION_DEPTH);

	//create the server-side player object
	NetworkPacket packet;
	packet.meta.type = NetworkPacket::Type::PLAYER_NEW;
	packet.playerInfo.clientIndex = clientIndex;
	snprintf(packet.playerInfo.handle, PACKET_STRING_SIZE, "%s", config["player.handle"].c_str());
	snprintf(packet.playerInfo.avatar, PACKET_STRING_SIZE, "%s", config["player.avatar"].c_str());
	packet.playerInfo.position = {0,0};
	packet.playerInfo.motion = {0,0};

	//send it
	char buffer[PACKET_BUFFER_SIZE];
	serialize(&packet, buffer);
	network.Send(Channels::SERVER, buffer, PACKET_BUFFER_SIZE);

	//request a sync
	packet.meta.type = NetworkPacket::Type::SYNCHRONIZE;
	serialize(&packet, buffer);
	network.Send(Channels::SERVER, buffer, PACKET_BUFFER_SIZE);

	//debug
//	RequestRegion(0, 0);
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

void InWorld::Update(double delta) {
	NetworkPacket packet;

	//suck in all waiting packets
	while(network.Receive()) {
		deserialize(&packet, network.GetInData());
		packet.meta.srcAddress = network.GetInPacket()->address;
		HandlePacket(packet);
	}

	//update the characters
	for (auto& it : playerCharacters) {
		it.second.Update(delta);
	}
	//TODO: sort the players and entities by Y position

	//update the camera
	if(localCharacter) {
		camera.x = localCharacter->GetPosition().x - camera.marginX;
		camera.y = localCharacter->GetPosition().y - camera.marginY;
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
}

void InWorld::Render(SDL_Surface* const screen) {
	//draw the map
	for (auto it = mapPager.GetContainer()->begin(); it != mapPager.GetContainer()->end(); it++) {
		tileSheet.DrawRegionTo(screen, *it, camera.x, camera.y);
	}

	//draw characters
	for (auto& it : playerCharacters) {
		it.second.DrawTo(screen, camera.x, camera.y);
	}

	//draw UI
	disconnectButton.DrawTo(screen);
	shutDownButton.DrawTo(screen);

	font.DrawStringTo(to_string_custom(fps.GetFrameRate()), screen, 0, 0);

	fps.Calculate();
}

//-------------------------
//Event handlers
//-------------------------

void InWorld::QuitEvent() {
	//exit the game AND the server
	RequestDisconnect();
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
	switch(key.keysym.sym) {
		case SDLK_ESCAPE: {
			QuitEvent();
		}
		break;

		//player movement
		case SDLK_LEFT:
			if (localCharacter) {
				localCharacter->AdjustDirection(PlayerCharacter::Direction::WEST);
				SendState();
			}
		break;

		case SDLK_RIGHT:
			if (localCharacter) {
				localCharacter->AdjustDirection(PlayerCharacter::Direction::EAST);
				SendState();
			}
		break;

		case SDLK_UP:
			if (localCharacter) {
				localCharacter->AdjustDirection(PlayerCharacter::Direction::NORTH);
				SendState();
			}
		break;

		case SDLK_DOWN:
			if (localCharacter) {
				localCharacter->AdjustDirection(PlayerCharacter::Direction::SOUTH);
				SendState();
			}
		break;
	}
}

void InWorld::KeyUp(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		//player movement
		case SDLK_LEFT:
			if (localCharacter) {
				localCharacter->AdjustDirection(PlayerCharacter::Direction::EAST);
				SendState();
			}
		break;

		case SDLK_RIGHT:
			if (localCharacter) {
				localCharacter->AdjustDirection(PlayerCharacter::Direction::WEST);
				SendState();
			}
		break;

		case SDLK_UP:
			if (localCharacter) {
				localCharacter->AdjustDirection(PlayerCharacter::Direction::SOUTH);
				SendState();
			}
		break;

		case SDLK_DOWN:
			if (localCharacter) {
				localCharacter->AdjustDirection(PlayerCharacter::Direction::NORTH);
				SendState();
			}
		break;
	}
}

//-------------------------
//Network handlers
//-------------------------

void InWorld::HandlePacket(NetworkPacket packet) {
	switch(packet.meta.type) {
		case NetworkPacket::Type::DISCONNECT:
			HandleDisconnect(packet);
		break;
		case NetworkPacket::Type::PLAYER_NEW:
			HandlePlayerNew(packet);
		break;
		case NetworkPacket::Type::PLAYER_DELETE:
			HandlePlayerDelete(packet);
		break;
		case NetworkPacket::Type::PLAYER_UPDATE:
			HandlePlayerUpdate(packet);
		break;
		case NetworkPacket::Type::REGION_CONTENT:
			HandleRegionContent(packet);
		break;
		//handle errors
		default:
			throw(std::runtime_error("Unknown NetworkPacket::Type encountered"));
		break;
	}
}

void InWorld::HandleDisconnect(NetworkPacket packet) {
	network.Unbind(Channels::SERVER);
	clientIndex = -1;
	SetNextScene(SceneList::MAINMENU);
}

void InWorld::HandlePlayerNew(NetworkPacket packet) {
	if (playerCharacters.find(packet.playerInfo.playerIndex) != playerCharacters.end()) {
		throw(std::runtime_error("Cannot create duplicate players"));
	}

	playerCharacters[packet.playerInfo.playerIndex].GetSprite()->LoadSurface(config["dir.sprites"] + packet.playerInfo.avatar, 4, 4);
	playerCharacters[packet.playerInfo.playerIndex].SetPosition(packet.playerInfo.position);
	playerCharacters[packet.playerInfo.playerIndex].SetMotion(packet.playerInfo.motion);
	playerCharacters[packet.playerInfo.playerIndex].ResetDirection();

	//catch this client's player object
	if (packet.playerInfo.clientIndex == clientIndex && !localCharacter) {
		playerIndex = packet.playerInfo.playerIndex;
		localCharacter = &playerCharacters[playerIndex];
		//setup the camera
		camera.width = GetScreen()->w;
		camera.height = GetScreen()->h;
		//center on the player's character
		camera.marginX = (GetScreen()->w / 2 - localCharacter->GetSprite()->GetImage()->GetClipW() / 2);
		camera.marginY = (GetScreen()->h / 2 - localCharacter->GetSprite()->GetImage()->GetClipH() / 2);
	}
}

void InWorld::HandlePlayerDelete(NetworkPacket packet) {
	if (playerCharacters.find(packet.playerInfo.playerIndex) == playerCharacters.end()) {
		throw(std::runtime_error("Cannot delete non-existant players"));
	}

	playerCharacters.erase(packet.playerInfo.playerIndex);

	//catch this client's player object
	if (packet.playerInfo.clientIndex == clientIndex) {
		playerIndex = -1;
		localCharacter = nullptr;
	}
}

void InWorld::HandlePlayerUpdate(NetworkPacket packet) {
	if (playerCharacters.find(packet.playerInfo.playerIndex) == playerCharacters.end()) {
		HandlePlayerNew(packet);
		return;
	}

	//update only if the message didn't originate from here
	if (packet.playerInfo.clientIndex != clientIndex) {
		playerCharacters[packet.playerInfo.playerIndex].SetPosition(packet.playerInfo.position);
		playerCharacters[packet.playerInfo.playerIndex].SetMotion(packet.playerInfo.motion);
	}
	playerCharacters[packet.playerInfo.playerIndex].ResetDirection();
}

void InWorld::HandleRegionContent(NetworkPacket packet) {
	//replace existing regions
	if (mapPager.FindRegion(packet.regionInfo.x, packet.regionInfo.y)) {
		mapPager.UnloadRegion(packet.regionInfo.x, packet.regionInfo.y);
	}
	mapPager.PushRegion(packet.regionInfo.region);
	packet.regionInfo.region = nullptr;

	//debugging
	cout << "Received region: " << packet.regionInfo.x << ", " << packet.regionInfo.y << endl;
	if (mapPager.FindRegion(packet.regionInfo.x, packet.regionInfo.y)) {
		cout << "Success" << endl;
	}
	else {
		cout << "Failure" << endl;
	}
}

//-------------------------
//Server control
//-------------------------

void InWorld::SendState() {
	NetworkPacket packet;
	char buffer[PACKET_BUFFER_SIZE];

	//pack the packet
	packet.meta.type = NetworkPacket::Type::PLAYER_UPDATE;
	packet.playerInfo.clientIndex = clientIndex;
	packet.playerInfo.playerIndex = playerIndex;
	packet.playerInfo.position = localCharacter->GetPosition();
	packet.playerInfo.motion = localCharacter->GetMotion();

	serialize(&packet, buffer);
	network.Send(Channels::SERVER, buffer, PACKET_BUFFER_SIZE);
}

void InWorld::RequestDisconnect() {
	NetworkPacket packet;
	char buffer[PACKET_BUFFER_SIZE];

	//send a disconnect request
	packet.meta.type = NetworkPacket::Type::DISCONNECT;
	packet.clientInfo.index = clientIndex;
	serialize(&packet, buffer);
	network.Send(Channels::SERVER, buffer, PACKET_BUFFER_SIZE);
}

void InWorld::RequestShutDown() {
	NetworkPacket packet;
	char buffer[PACKET_BUFFER_SIZE];

	//send a shutdown request
	packet.meta.type = NetworkPacket::Type::SHUTDOWN;
	packet.clientInfo.index = clientIndex;
	serialize(&packet, buffer);
	network.Send(Channels::SERVER, buffer, PACKET_BUFFER_SIZE);
}

void InWorld::RequestRegion(int x, int y) {
	NetworkPacket packet;
	char buffer[PACKET_BUFFER_SIZE];

	//pack the region's data
	packet.meta.type = NetworkPacket::Type::REGION_REQUEST;
	packet.regionInfo.x = x;
	packet.regionInfo.y = y;
	serialize(&packet, buffer);
	network.Send(Channels::SERVER, buffer, PACKET_BUFFER_SIZE);
}

//-------------------------
//Utilities
//-------------------------

int InWorld::CheckBufferDistance(Region* const region) {
	/* DOCUMENTATION
	 * This algorithm is extremely complex and involed, but initial tests show
	 * that it gives the right answers. The purpose is to determine how far off screen
	 * a certain region is, so that it can be unloaded when necessary.
	 * 
	 * If the region is actually onscreen, then there's no reason to run the rest, so
	 * the algorithm corrects for the camera's location, before checking the bounds of
	 * the screen.
	 * 
	 * The next part is tricky. If X or Y is negative, then it is divided by the
	 * graphical size of the regions, resulting in a usable integer, representing how
	 * far from the screen it is in "region units". If, however, X or Y is larger than
	 * 0, than first, the size of the screen is subtracted from that variable, before
	 * it is then divided by the graphical size of a region. Finally, to compensate for
	 * the off by one error, 1 is added at the end.
	 *
	 * Since only the magnitude of the distance in either direction is needed, this
	 * method returns the largest absolute value of X or Y.
	 * 
	 * The final result of this algorithm is an integer representing how far, rounded
	 * up, a certain region is from the screen's edges in any direction, measured in
	 * "region units". This algorithm may be flawed, in which case, I recommend simply
	 * replacing it with a boolean check, to see if the region's distance from the player
	 * is larger than a certain value. This algorithm lacks the advantages I initially
	 * expected, so that may be beneficial at some point.
	*/

	//locations relative to the camera
	int x = region->GetX() - camera.x;
	int y = region->GetY() - camera.y;

	//if the region is visible, return -1
	if (x >= -mapPager.GetRegionWidth() * tileSheet.GetTileW() && x < camera.width &&
		y >= -mapPager.GetRegionHeight() * tileSheet.GetTileH() && y < camera.height) {
		return -1;
	}

	//prune the screen's area from the algorithm; get the pseudo-indexes
	if (x < 0) x /= (mapPager.GetRegionWidth() * tileSheet.GetTileW());
	if (y < 0) y /= (mapPager.GetRegionHeight() * tileSheet.GetTileH());
	if (x > 0) x = (x - camera.width) / (mapPager.GetRegionWidth() * tileSheet.GetTileW()) + 1;
	if (y > 0) y = (y - camera.height) / (mapPager.GetRegionHeight() * tileSheet.GetTileH()) + 1;

	//return the pseudo-index with the greatest magnitude
	return std::max(abs(x), abs(y));
}

//TODO: eew ugly
void InWorld::UpdateMap() {
	//prune distant regions
	for (auto it = mapPager.GetContainer()->begin(); it != mapPager.GetContainer()->end(); /* EMPTY */) {
		if (CheckBufferDistance(*it) > 2) {
			//debugging
			cout << "unloading: " << (*it)->GetX() << ", " << (*it)->GetY() << endl;

			mapPager.UnloadRegion((*it)->GetX(), (*it)->GetY());

			//reset
			it = mapPager.GetContainer()->begin();
			continue;
		}
		++it;
	}

	//TODO: make the region units official
	int regionUnitX = mapPager.GetRegionWidth() * tileSheet.GetTileW();
	int regionUnitY = mapPager.GetRegionHeight() * tileSheet.GetTileH();

	//request empty regions, including buffers (-1 & +1 region unit)
	for (int i = snapToBase(regionUnitX, camera.x - regionUnitX); i <= snapToBase(regionUnitX, camera.x + camera.width + regionUnitX); i += regionUnitX) {
		for (int j = snapToBase(regionUnitY, camera.y - regionUnitY); j <= snapToBase(regionUnitY, camera.y + camera.height + regionUnitY); j += regionUnitY) {
			if (!mapPager.FindRegion(i, j)) {
				RequestRegion(i, j);
			}
		}
	}
}
