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
#include "in_combat.hpp"

#include "channels.hpp"
#include "utility.hpp"

#include <stdexcept>

//-------------------------
//Public access members
//-------------------------

InCombat::InCombat(
	ConfigUtility* const argConfig,
	UDPNetworkUtility* const argNetwork,
	int* const argClientIndex,
	int* const argAccountIndex,
	int* const argCharacterIndex,
	std::map<int, CombatData>* argCombatMap,
	std::map<int, CharacterData>* argCharacterMap,
	std::map<int, EnemyData>* argEnemyMap
	):
	config(*argConfig),
	network(*argNetwork),
	clientIndex(*argClientIndex),
	accountIndex(*argAccountIndex),
	characterIndex(*argCharacterIndex),
	combatMap(*argCombatMap),
	characterMap(*argCharacterMap),
	enemyMap(*argEnemyMap)
{
/*	//setup the utility objects
	buttonImage.LoadSurface(config["dir.interface"] + "button_menu.bmp");
	buttonImage.SetClipH(buttonImage.GetClipH()/3);
	font.LoadSurface(config["dir.fonts"] + "pk_white_8.bmp");

	//pass the utility objects
	backButton.SetImage(&buttonImage);
	backButton.SetFont(&font);

	//set the button positions
	backButton.SetX(50);
	backButton.SetY(50 + buttonImage.GetClipH() * 0);

	//set the button texts
	backButton.SetText("Back");

	//request a sync
	RequestSynchronize();
*/
	//debug
	//
}

InCombat::~InCombat() {
	//
}

//-------------------------
//Frame loop
//-------------------------

void InCombat::FrameStart() {
	//
}

void InCombat::Update(double delta) {
	SerialPacket packet;

	//suck in all waiting packets
	while(network.Receive(&packet)) {
		HandlePacket(packet);
	}

	//TODO: more
}

void InCombat::FrameEnd() {
	//
}

void InCombat::RenderFrame() {
	SDL_FillRect(GetScreen(), 0, 0);
	Render(GetScreen());
	SDL_Flip(GetScreen());
	fps.Calculate();
}

void InCombat::Render(SDL_Surface* const screen) {
	//TODO: draw the background

	//TODO: draw the characters

	//TODO: draw the enemies

	//TODO: draw the UI
}

//-------------------------
//Event handlers
//-------------------------

void InCombat::QuitEvent() {
	//exit the game AND the server
	RequestDisconnect();
	SetNextScene(SceneList::MAINMENU);
}

void InCombat::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void InCombat::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//
}

void InCombat::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void InCombat::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
			break;
	}
}

void InCombat::KeyUp(SDL_KeyboardEvent const& key) {
	//
}

//-------------------------
//Network handlers
//-------------------------

void InCombat::HandlePacket(SerialPacket packet) {
	switch(packet.meta.type) {
		case SerialPacket::Type::DISCONNECT:
			HandleDisconnect(packet);
		break;
		//handle errors
		default:
			throw(std::runtime_error(std::string() + "Unknown SerialPacket::Type encountered in InCombat: " + to_string_custom(int(packet.meta.type))));
		break;
	}
}

void InCombat::HandleDisconnect(SerialPacket) {
	SetNextScene(SceneList::RESTART);
}

//TODO: more network handlers

//-------------------------
//Server control
//-------------------------

void InCombat::RequestSynchronize() {
	SerialPacket packet;

	//request a sync
	packet.meta.type = SerialPacket::Type::SYNCHRONIZE;
	packet.clientInfo.clientIndex = clientIndex;
	packet.clientInfo.accountIndex = accountIndex;
	packet.clientInfo.characterIndex = characterIndex;

	network.SendTo(Channels::SERVER, &packet);
}

void InCombat::SendPlayerUpdate() {
	SerialPacket packet;

	//pack the packet
	packet.meta.type = SerialPacket::Type::CHARACTER_UPDATE;
	packet.characterInfo.clientIndex = clientIndex;
	packet.characterInfo.accountIndex = accountIndex;
	packet.characterInfo.characterIndex = characterIndex;
//	packet.characterInfo.position = localCharacter->position;
//	packet.characterInfo.motion = localCharacter->motion;
	//TODO: stats

	network.SendTo(Channels::SERVER, &packet);
}

void InCombat::RequestDisconnect() {
	SerialPacket packet;

	//send a disconnect request
	packet.meta.type = SerialPacket::Type::DISCONNECT;
	packet.clientInfo.clientIndex = clientIndex;
	packet.clientInfo.accountIndex = accountIndex;
	packet.clientInfo.characterIndex = characterIndex;

	network.SendTo(Channels::SERVER, &packet);
}

void InCombat::RequestShutdown() {
	SerialPacket packet;

	//send a shutdown request
	packet.meta.type = SerialPacket::Type::SHUTDOWN;
	packet.clientInfo.clientIndex = clientIndex;
	packet.clientInfo.accountIndex = accountIndex;
	packet.clientInfo.characterIndex = characterIndex;

	network.SendTo(Channels::SERVER, &packet);
}
