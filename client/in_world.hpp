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
#ifndef INWORLD_HPP_
#define INWORLD_HPP_

#include "base_scene.hpp"

#include "utilities.hpp"
#include "defines.hpp"
#include "singleton.hpp"
#include "packet.hpp"
#include "network_queue.hpp"
#include "information_manager.hpp"
#include "player_character.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "udp_network_utility.hpp"
#include "button.hpp"
#include "raster_font.hpp"
#include "frame_rate.hpp"

#include <map>

class InWorld : public BaseScene {
public:
	//Public access members
	InWorld();
	~InWorld();

protected:
	//Frame loop
	void FrameStart();
	void Update(double delta);
	void FrameEnd();
	void Render(SDL_Surface* const);

	//Event handlers
	void QuitEvent();
	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	void MouseButtonUp(SDL_MouseButtonEvent const&);
	void KeyDown(SDL_KeyboardEvent const&);
	void KeyUp(SDL_KeyboardEvent const&);

	//Utilities
	int HandlePacket(Packet::Packet);
	void Disconnect();
	void ExitGame();

	void HandleDisconnection(Packet::Disconnect&);

	void AddPlayer(Packet::PlayerData&);
	void RemovePlayer(Packet::PlayerDelete&);
	void UpdatePlayer(Packet::PlayerData&);

	//services
	ConfigUtility* configUtil = Singleton<ConfigUtility>::Get();
	SurfaceManager* surfaceMgr = Singleton<SurfaceManager>::Get();
	UDPNetworkUtility* netUtil = Singleton<UDPNetworkUtility>::Get();
	InformationManager* infoMgr = Singleton<InformationManager>::Get();

	//members
	RasterFont font;
	std::map<int, PlayerCharacter> playerCharacters;
};

#endif
