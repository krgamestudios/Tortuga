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
	//suck in and process all waiting packets
	char packetBuffer[MAX_PACKET_SIZE];
	while(network.Receive(reinterpret_cast<SerialPacket*>(packetBuffer))) {
		HandlePacket(reinterpret_cast<SerialPacket*>(packetBuffer));
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

void InCombat::HandlePacket(SerialPacket* const argPacket) {
	switch(argPacket->type) {
		case SerialPacketType::DISCONNECT:
			HandleDisconnect(argPacket);
		break;
		//handle errors
		default:
			throw(std::runtime_error(std::string() + "Unknown SerialPacketType encountered in InCombat: " + to_string_custom(static_cast<int>(argPacket->type)) ));
		break;
	}
}

void InCombat::HandleDisconnect(SerialPacket* const) {
	SetNextScene(SceneList::RESTART);
}

//TODO: more network handlers

//-------------------------
//Server control
//-------------------------

void InCombat::RequestSynchronize() {
	ClientPacket newPacket;

	//request a sync
	newPacket.type = SerialPacketType::SYNCHRONIZE;
	newPacket.clientIndex = clientIndex;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InCombat::SendPlayerUpdate() {
	CharacterPacket newPacket;

	//pack the packet
	newPacket.type = SerialPacketType::CHARACTER_UPDATE;

	newPacket.characterIndex = characterIndex;
	//handle, avatar
	newPacket.accountIndex = accountIndex;
//	newPacket.roomIndex = localCharacter->roomIndex;
//	newPacket.origin = localCharacter->origin;
//	newPacket.motion = localCharacter->motion;
//	newPacket.stats = localCharacter->stats;

	//TODO: equipment
	//TODO: items
	//TODO: buffs
	//TODO: debuffs

	network.SendTo(Channels::SERVER, &newPacket);
}

void InCombat::RequestDisconnect() {
	ClientPacket newPacket;

	//send a disconnect request
	newPacket.type = SerialPacketType::DISCONNECT;
	newPacket.clientIndex = clientIndex;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InCombat::RequestShutdown() {
	ClientPacket newPacket;

	//send a shutdown request
	newPacket.type = SerialPacketType::SHUTDOWN;
	newPacket.clientIndex = clientIndex;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}
