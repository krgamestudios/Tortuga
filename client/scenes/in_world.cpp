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
}

void InWorld::FrameEnd() {
	//
}

void InWorld::Render(SDL_Surface* const screen) {
	disconnectButton.DrawTo(screen);
	shutDownButton.DrawTo(screen);
}

//-------------------------
//Event handlers
//-------------------------

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
		//send a disconnect request
		NetworkPacket packet;
		packet.meta.type = NetworkPacket::Type::DISCONNECT;
		packet.clientInfo.index = clientIndex;
		network.Send(Channels::SERVER, &packet, sizeof(NetworkPacket));
	}
	if (shutDownButton.MouseButtonUp(button) == Button::State::HOVER) {
		//send a shutdown request
		NetworkPacket packet;
		packet.meta.type = NetworkPacket::Type::SHUTDOWN;
		network.Send(Channels::SERVER, &packet, sizeof(NetworkPacket));
	}
}

void InWorld::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			//send a disconnect request
			NetworkPacket packet;
			packet.meta.type = NetworkPacket::Type::DISCONNECT;
			packet.clientInfo.index = clientIndex;
			network.Send(Channels::SERVER, &packet, sizeof(NetworkPacket));
			QuitEvent();
			break;
	}
}

void InWorld::KeyUp(SDL_KeyboardEvent const& key) {
	//
}

void InWorld::HandlePacket(NetworkPacket packet) {
		switch(packet.meta.type) {
		case NetworkPacket::Type::DISCONNECT:
			network.Unbind(Channels::SERVER);
			clientIndex = -1;
			SetNextScene(SceneList::MAINMENU);
		break;
		case NetworkPacket::Type::SYNCHRONIZE:
			//TODO
		break;

		//handle errors
		default:
			throw(std::runtime_error("Unknown NetworkPacket::Type encountered"));
		break;
	}
}