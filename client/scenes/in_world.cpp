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

#include <stdexcept>

//-------------------------
//Public access members
//-------------------------

InWorld::InWorld(ConfigUtility* const arg1, UDPNetworkUtility* const arg2, int* const arg3):
	config(*arg1),
	network(*arg2),
	clientIndex(*arg3)
{
	//setup the utility objects
	image.LoadSurface(config["dir.interface"] + "button_menu.bmp");
	image.SetClipH(image.GetClipH()/3);
	font.LoadSurface(config["dir.fonts"] + "pk_white_8.bmp");

	//pass the utility objects
	disconnectButton.SetImage(&image);
	disconnectButton.SetFont(&font);
	shutDownButton.SetImage(&image);
	shutDownButton.SetFont(&font);

	//set the button positions
	disconnectButton.SetX(50);
	disconnectButton.SetY(50 + image.GetClipH() * 0);
	shutDownButton.SetX(50);
	shutDownButton.SetY(50 + image.GetClipH() * 1);

	//set the button texts
	disconnectButton.SetText("Disconnect");
	shutDownButton.SetText("Shut Down");

	//create the server-side player object
	NetworkPacket packet;
	packet.meta.type = NetworkPacket::Type::PLAYER_NEW;
	packet.playerInfo.clientIndex = clientIndex;
	snprintf(packet.playerInfo.handle, PACKET_STRING_SIZE, "%s", config["player.handle"].c_str());
	snprintf(packet.playerInfo.avatar, PACKET_STRING_SIZE, "%s", config["player.avatar"].c_str());
	packet.playerInfo.position = {0,0};
	packet.playerInfo.motion = {0,0};

	//send it
	network.Send(Channels::SERVER, &packet, sizeof(NetworkPacket));

	//request a sync
	packet.meta.type = NetworkPacket::Type::SYNCHRONIZE;
	network.Send(Channels::SERVER, &packet, sizeof(NetworkPacket));
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
	//suck in all waiting packets
	NetworkPacket packet;
	while(network.Receive()) {
		memcpy(&packet, network.GetInData(), sizeof(NetworkPacket));
		packet.meta.srcAddress = network.GetInPacket()->address;
		HandlePacket(packet);
	}

	for (auto& it : playerCharacters) {
		it.second.Update(delta);
	}
}

void InWorld::FrameEnd() {
	//
}

void InWorld::Render(SDL_Surface* const screen) {
	for (auto& it : playerCharacters) {
		it.second.DrawTo(screen);
	}
	disconnectButton.DrawTo(screen);
	shutDownButton.DrawTo(screen);
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

void InWorld::SendState() {
	NetworkPacket packet;
	packet.meta.type = NetworkPacket::Type::PLAYER_UPDATE;
	packet.playerInfo.clientIndex = clientIndex;
	packet.playerInfo.playerIndex = playerIndex;
//	snprintf(packet.playerInfo.handle, PACKET_STRING_SIZE, "%s", config["player.handle"].c_str());
//	snprintf(packet.playerInfo.avatar, PACKET_STRING_SIZE, "%s", config["player.avatar"].c_str());
	packet.playerInfo.position = localCharacter->GetPosition();
	packet.playerInfo.motion = localCharacter->GetMotion();

	network.Send(Channels::SERVER, &packet, sizeof(NetworkPacket));
}

void InWorld::RequestDisconnect() {
	//send a disconnect request
	NetworkPacket packet;
	packet.meta.type = NetworkPacket::Type::DISCONNECT;
	packet.clientInfo.index = clientIndex;
	network.Send(Channels::SERVER, &packet, sizeof(NetworkPacket));
}

void InWorld::RequestShutDown() {
	//send a shutdown request
	NetworkPacket packet;
	packet.meta.type = NetworkPacket::Type::SHUTDOWN;
	packet.clientInfo.index = clientIndex;
	network.Send(Channels::SERVER, &packet, sizeof(NetworkPacket));
}