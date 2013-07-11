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

	//debugging
	Packet p;
	p.meta.type = Packet::Type::PLAYER_NEW;
	p.meta.clientIndex = infoMgr->GetClientIndex();

	snprintf(p.playerInfo.handle, PACKET_STRING_SIZE, "%s", configUtil->CString("handle"));
	snprintf(p.playerInfo.avatar, PACKET_STRING_SIZE, "%s", configUtil->CString("avatar"));
	p.playerInfo.position.x = 50;
	p.playerInfo.position.y = 50;
	p.playerInfo.motion.x = 0;
	p.playerInfo.motion.y = 0;

	netUtil->Send(GAME_CHANNEL, &p, sizeof(Packet));

	//request a sync
	p.meta.type = Packet::Type::SYNCHRONIZE;
	p.meta.clientIndex = infoMgr->GetClientIndex();
	netUtil->Send(GAME_CHANNEL, &p, sizeof(Packet));
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

	for (auto& it : playerCharacters) {
		it.second.Update(delta);
	}
}

void InWorld::FrameEnd() {
	//
}

void InWorld::Render(SDL_Surface* const screen) {
	ClockFrameRate();

	for (auto& it : playerCharacters) {
		it.second.DrawTo(screen);
	}

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
	//general
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			ExitGame();
		break;
	}

	//player movement
	if (infoMgr->GetPlayerIndex() == -1) {
		return;
	}

	switch(key.keysym.sym) {
		case SDLK_w:
			playerCharacters[infoMgr->GetPlayerIndex()].MoveDirection(CardinalDirection::NORTH);
			SendState();
		break;
		case SDLK_s:
			playerCharacters[infoMgr->GetPlayerIndex()].MoveDirection(CardinalDirection::SOUTH);
			SendState();
		break;
		case SDLK_a:
			playerCharacters[infoMgr->GetPlayerIndex()].MoveDirection(CardinalDirection::WEST);
			SendState();
		break;
		case SDLK_d:
			playerCharacters[infoMgr->GetPlayerIndex()].MoveDirection(CardinalDirection::EAST);
			SendState();
		break;
	}
}

void InWorld::KeyUp(SDL_KeyboardEvent const& key) {
	//player movement reversed
	switch(key.keysym.sym) {
		case SDLK_w:
			playerCharacters[infoMgr->GetPlayerIndex()].MoveDirection(CardinalDirection::SOUTH);
			SendState();
		break;
		case SDLK_s:
			playerCharacters[infoMgr->GetPlayerIndex()].MoveDirection(CardinalDirection::NORTH);
			SendState();
		break;
		case SDLK_a:
			playerCharacters[infoMgr->GetPlayerIndex()].MoveDirection(CardinalDirection::EAST);
			SendState();
		break;
		case SDLK_d:
			playerCharacters[infoMgr->GetPlayerIndex()].MoveDirection(CardinalDirection::WEST);
			SendState();
		break;
	}
}

//-------------------------
//Utilities
//-------------------------

int InWorld::HandlePacket(Packet p) {
	switch(p.meta.type) {
		case Packet::Type::NONE:
			//DO NOTHING
			return 0;
		break;
		case Packet::Type::PING:
			//quick pong
			p.meta.type = Packet::Type::PONG;
			p.meta.clientIndex = infoMgr->GetClientIndex();
			netUtil->Send(&p.meta.address, &p, sizeof(Packet));
		break;
		case Packet::Type::PONG:
			//
		break;
//		case Packet::Type::BROADCAST_REQUEST:
//			//
//		break;
//		case Packet::Type::BROADCAST_RESPONSE:
//			//
//		break;
//		case Packet::Type::JOIN_REQUEST:
//			//
//		break;
//		case Packet::Type::JOIN_RESPONSE:
//			//
//		break;
		case Packet::Type::DISCONNECT:
			HandleDisconnection(p);
		break;
//		case Packet::Type::SYNCHRONIZE:
//			//
//		break;
		case Packet::Type::PLAYER_NEW:
			AddPlayer(p);
		break;
		case Packet::Type::PLAYER_DELETE:
			RemovePlayer(p);
		break;
		case Packet::Type::PLAYER_UPDATE:
			UpdatePlayer(p);
		break;
		default:
			throw(runtime_error("Failed to recognize the packet type: " + itos(int(p.meta.type))));
	}
	return 1;
}

void InWorld::Disconnect() {
	Packet p;

	//delete the player
	p.meta.type = Packet::Type::PLAYER_DELETE;
	p.meta.clientIndex = infoMgr->GetClientIndex();
	p.playerInfo.index = infoMgr->GetPlayerIndex();
	netUtil->Send(GAME_CHANNEL, &p, sizeof(Packet));

	//disconnect
	p.meta.type = Packet::Type::DISCONNECT;
	p.meta.clientIndex = infoMgr->GetClientIndex();
	netUtil->Send(GAME_CHANNEL, &p, sizeof(Packet));

	netUtil->Unbind(GAME_CHANNEL);

	//reset the client
	infoMgr->ResetClientIndex();
	infoMgr->ResetPlayerIndex();
}

void InWorld::ExitGame() {
	Disconnect();
	SetNextScene(SceneList::MAINMENU);
	endQueueThread();
	cout << "The game session has ended" << endl;
}

void InWorld::HandleDisconnection(Packet& disconnect) {
	Disconnect();
	SetNextScene(SceneList::MAINMENU);
	endQueueThread();
	cout << "You have been disconnected" << endl;
}

void InWorld::AddPlayer(Packet& p) {
	if (playerCharacters.find(p.playerInfo.index) != playerCharacters.end()) {
		throw(runtime_error("Duplicate players detected"));
	}

	//position
	playerCharacters[p.playerInfo.index].SetPosition(p.playerInfo.position);
	playerCharacters[p.playerInfo.index].SetMotion(p.playerInfo.motion);

	//sprite
	playerCharacters[p.playerInfo.index].GetSprite()->SetSurface(surfaceMgr->Get(p.playerInfo.avatar), 32, 48);
	playerCharacters[p.playerInfo.index].FaceDirection();

	//is it this player?
	if (p.meta.clientIndex == infoMgr->GetClientIndex()) {
		infoMgr->SetPlayerIndex(p.playerInfo.index);
	}

	//debugging
	cout << "New player, index " << p.playerInfo.index << endl;
}

void InWorld::RemovePlayer(Packet& p) {
	if (playerCharacters.find(p.playerInfo.index) == playerCharacters.end()) {
		throw(runtime_error("Player to delete not found"));
	}

	playerCharacters.erase(p.playerInfo.index);
}

void InWorld::UpdatePlayer(Packet& p) {
	if (playerCharacters.find(p.playerInfo.index) == playerCharacters.end()) {
		AddPlayer(p);
		return;
	}

	playerCharacters[p.playerInfo.index].SetPosition(p.playerInfo.position);
	playerCharacters[p.playerInfo.index].SetMotion(p.playerInfo.motion);
	playerCharacters[p.playerInfo.index].FaceDirection();
}

void InWorld::SendState() {
	//send the state of this player's character
	if (infoMgr->GetPlayerIndex() == -1) {
		return;
	}

	Packet p;
	p.meta.type = Packet::Type::PLAYER_UPDATE;
	p.meta.clientIndex = infoMgr->GetClientIndex();
	p.playerInfo.index = infoMgr->GetPlayerIndex();
	p.playerInfo.position = playerCharacters[infoMgr->GetPlayerIndex()].GetPosition();
	p.playerInfo.motion = playerCharacters[infoMgr->GetPlayerIndex()].GetMotion();

	netUtil->Send(GAME_CHANNEL, &p, sizeof(Packet));
}
