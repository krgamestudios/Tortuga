/* Copyright: (c) Kayne Ruse 2013-2015
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
#pragma once

//maps
#include "region_pager_base.hpp"

//utilities
#include "udp_network_utility.hpp"
#include "serial_packet.hpp"
#include "config_utility.hpp"

//graphics
#include "image.hpp"
#include "button.hpp"
#include "tile_sheet.hpp"
#include "text_line.hpp"

//common
#include "frame_rate.hpp"

//client
#include "base_scene.hpp"
#include "base_creature.hpp"
#include "local_character.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#include "SDL2/SDL_ttf.h"

//STL
#include <map>

#include <chrono>

class World: public BaseScene {
public:
	//Public access members
	World(int* const argClientIndex, int* const argAccountIndex);
	~World();

	void RenderFrame(SDL_Renderer* renderer) override;

private:
	//frame phases
	void FrameStart() override;
	void Update() override;
	void FrameEnd() override;

	//input events
	void QuitEvent();
	void MouseMotion(SDL_MouseMotionEvent const& event) override;
	void MouseButtonDown(SDL_MouseButtonEvent const& event) override;
	void MouseButtonUp(SDL_MouseButtonEvent const& event) override;
	void MouseWheel(SDL_MouseWheelEvent const& event) override;
	void KeyDown(SDL_KeyboardEvent const& event) override;
	void KeyUp(SDL_KeyboardEvent const& event) override;

	//handle incoming traffic
	void HandlePacket(SerialPacket* const);

	//heartbeat system
	void hPing(ServerPacket* const);
	void hPong(ServerPacket* const);

	void CheckHeartBeat();

	//basic connections
	void SendLogoutRequest();
	void SendDisconnectRequest();
	void SendAdminDisconnectForced();
	void SendAdminShutdownRequest();

	void hLogoutResponse(ClientPacket* const);
	void hDisconnectResponse(ClientPacket* const);
	void hAdminDisconnectForced(ClientPacket* const);

	//map management
	void SendRegionRequest(int roomIndex, int x, int y);
	void hRegionContent(RegionPacket* const);
	void UpdateMap();

	//character management
	void hCharacterUpdate(CharacterPacket* const);
	void hCharacterCreate(CharacterPacket* const);
	void hCharacterUnload(CharacterPacket* const);
	void hQueryCharacterExists(CharacterPacket* const);
	void hCharacterMovement(CharacterPacket* const);

	//creature management
	void hCreatureCreate(CreaturePacket* const);
	void hCreatureUnload(CreaturePacket* const);
	void hQueryCreatureExists(CreaturePacket* const);
	void hCreatureMovement(CreaturePacket* const);

	//chat
	//TODO: ui chat engine
	void hTextBroadcast(TextPacket* const);
	void hTextSpeech(TextPacket* const);
	void hTextWhisper(TextPacket* const);

	//general gameplay
	void SendLocalCharacterMovement();
	std::list<BoundingBox> GenerateCollisionGrid(Entity*, int tileWidth, int tileHeight);

	//indexes
	int& clientIndex;
	int& accountIndex;
	int characterIndex = -1;
	int roomIndex = -1;

	//graphics
	TileSheet tileSheet;

	//map
	RegionPagerBase regionPager;

	//UI
	Image buttonImage;
	TTF_Font* font = nullptr;
	Button disconnectButton;
	Button shutdownButton;
	FrameRate fps;
	TextLine fpsTextLine;

	//the camera structure
	struct {
		int x = 0, y = 0;
		int width = 0, height = 0;
		int marginX = 0, marginY = 0;
	} camera;

	//entities
	std::map<int, BaseCharacter> characterMap;
	std::map<int, BaseCreature> creatureMap;
	LocalCharacter* localCharacter = nullptr;

	//heartbeat
	//TODO: (2) Heartbeat needs it's own utility
	typedef std::chrono::steady_clock Clock;
	Clock::time_point lastBeat = Clock::now();
	int attemptedBeats = 0;

	//ugly references; I hate this
	ConfigUtility& config = ConfigUtility::GetSingleton();
	UDPNetworkUtility& network = UDPNetworkUtility::GetSingleton();
};
