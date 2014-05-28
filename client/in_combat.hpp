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
#ifndef INCOMBAT_HPP_
#define INCOMBAT_HPP_

//network
#include "udp_network_utility.hpp"

//graphics
#include "image.hpp"
#include "raster_font.hpp"
#include "button.hpp"

//common
#include "config_utility.hpp"
#include "frame_rate.hpp"

#include "combat_data.hpp"
#include "character_data.hpp"
#include "enemy_data.hpp"

//client
#include "base_scene.hpp"

class InCombat : public BaseScene {
public:
	//Public access members
	InCombat(
		ConfigUtility* const argConfig,
		UDPNetworkUtility* const argNetwork,
		int* const argClientIndex,
		int* const argAccountIndex,
		int* const argCharacterIndex,
		std::map<int, CombatData>* argCombatMap,
		std::map<int, CharacterData>* argCharacterMap,
		std::map<int, EnemyData>* argEnemyMap
	);
	~InCombat();

protected:
	//Frame loop
	void FrameStart();
	void Update(double delta);
	void FrameEnd();
	void RenderFrame();
	void Render(SDL_Surface* const);

	//Event handlers
	void QuitEvent();
	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	void MouseButtonUp(SDL_MouseButtonEvent const&);
	void KeyDown(SDL_KeyboardEvent const&);
	void KeyUp(SDL_KeyboardEvent const&);

	//Network handlers
	void HandlePacket(SerialPacket);
	void HandleDisconnect(SerialPacket);
	//TODO: more

	//Server control
	void SendPlayerUpdate();
	void RequestDisconnect();
	void RequestShutdown();
	//TODO: more

	//shared parameters
	ConfigUtility& config;
	UDPNetworkUtility& network;
	int& clientIndex;
	int& accountIndex;
	int& characterIndex;
	std::map<int, CombatData>& combatMap;
	std::map<int, CharacterData>& characterMap;
	std::map<int, EnemyData>& enemyMap;

	//graphics
	//TODO: graphics

	//UI
	//TODO: UI
	FrameRate fps;
};

#endif
