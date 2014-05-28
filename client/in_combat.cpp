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
	//
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
	//
}

//-------------------------
//Event handlers
//-------------------------

void InCombat::QuitEvent() {
	//exit the game AND the server
//	RequestDisconnect();
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

//TODO: network handlers

//-------------------------
//Server control
//-------------------------

//TODO: server control
