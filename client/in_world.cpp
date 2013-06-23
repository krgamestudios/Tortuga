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

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

InWorld::InWorld() {
#ifdef DEBUG
	cout << "entering InWorld" << endl;
#endif
	cout << "Client Index: " << infoMgr->GetClientIndex() << endl;
	font.SetSurface(surfaceMgr->Get("font"));
	pc.GetSprite()->SetSurface(surfaceMgr->Get("elliot"), 32, 48);
}

InWorld::~InWorld() {
#ifdef DEBUG
	cout << "leaving InWorld" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void InWorld::FrameStart() {
	//
}

void InWorld::Update(double delta) {
	while(HandlePacket(popNetworkPacket()));
	pc.Update(delta);
}

void InWorld::FrameEnd() {
	//
}

void InWorld::Render(SDL_Surface* const screen) {
	ClockFrameRate();

	pc.DrawTo(screen);

	//since we're using this twice, make a tmp var
	string fps = itos(GetFrameRate());
	font.DrawStringTo(fps, screen, screen->w - fps.size() * font.GetCharW(), 0);
}

//-------------------------
//Event handlers
//-------------------------

void InWorld::QuitEvent() {
	//ensure that the client is disconnected properly
	ExitGame();
	SetNextScene(SceneList::QUIT);
}

void InWorld::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void InWorld::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void InWorld::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void InWorld::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			ExitGame();
			break;
		case SDLK_w:
			pc.MoveDirection(CardinalDirection::NORTH);
		break;
		case SDLK_s:
			pc.MoveDirection(CardinalDirection::SOUTH);
		break;
		case SDLK_a:
			pc.MoveDirection(CardinalDirection::EAST);
		break;
		case SDLK_d:
			pc.MoveDirection(CardinalDirection::WEST);
		break;
	}
}

void InWorld::KeyUp(SDL_KeyboardEvent const& key) {
	//reversed
	switch(key.keysym.sym) {
		case SDLK_w:
			pc.MoveDirection(CardinalDirection::SOUTH);
		break;
		case SDLK_s:
			pc.MoveDirection(CardinalDirection::NORTH);
		break;
		case SDLK_a:
			pc.MoveDirection(CardinalDirection::WEST);
		break;
		case SDLK_d:
			pc.MoveDirection(CardinalDirection::EAST);
		break;
	}
}

//-------------------------
//Utilities
//-------------------------

int InWorld::HandlePacket(Packet p) {
	switch(p.meta.type) {
		case PacketType::NONE:
			//DO NOTHING
			return 0;
		break;
		case PacketType::PING:
			//quick pong
			p.meta.type = PacketType::PONG;
			netUtil->Send(&p.meta.address, &p, sizeof(Packet));
		break;
		case PacketType::PONG:
			//
		break;
//		case PacketType::BROADCAST_REQUEST:
//			//
//		break;
//		case PacketType::BROADCAST_RESPONSE:
//			//
//		break;
//		case PacketType::JOIN_REQUEST:
//			//
//		break;
//		case PacketType::JOIN_RESPONSE:
//			//
//		break;
		case PacketType::DISCONNECT:
			HandleDisconnection(p.disconnect);
		break;
//		case PacketType::SYNCHRONIZE:
//			//
//		break;
//		case PacketType::PLAYER_NEW:
//			//
//		break;
//		case PacketType::PLAYER_DELETE:
//			//
//		break;
//		case PacketType::PLAYER_MOVE:
//			//
//		break;
		default:
			throw(runtime_error("Failed to recognize the packet type: " + itos(int(p.meta.type))));
	}
	return 1;
}

void InWorld::Disconnect() {
	//disconnect
	Packet p;
	p.meta.type = PacketType::DISCONNECT;
	p.disconnect.clientIndex = infoMgr->GetClientIndex();
	netUtil->Send(GAME_CHANNEL, reinterpret_cast<void*>(&p), sizeof(Packet));
	netUtil->Unbind(GAME_CHANNEL);
	endQueueThread();

	//reset the client
	infoMgr->ResetClientIndex();
}

void InWorld::ExitGame() {
	Disconnect();
	SetNextScene(SceneList::MAINMENU);
	cout << "The game session has ended" << endl;
}

void InWorld::HandleDisconnection(::Disconnect& disconnect) {
	Disconnect();
	SetNextScene(SceneList::MAINMENU);
	cout << "You have been disconnected" << endl;
}
