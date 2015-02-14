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
#ifndef INWORLD_HPP_
#define INWORLD_HPP_

//maps
#include "region_pager_base.hpp"

//utilities
#include "udp_network_utility.hpp"
#include "serial_packet.hpp"
#include "config_utility.hpp"

//graphics
#include "image.hpp"
#include "raster_font.hpp"
#include "button.hpp"
#include "tile_sheet.hpp"

//common
#include "frame_rate.hpp"

//client
#include "base_scene.hpp"
#include "base_monster.hpp"
#include "local_character.hpp"

//STL
#include <map>

#include <chrono>

class World: public BaseScene {
public:
	//Public access members
	World(int* const argClientIndex, int* const argAccountIndex);
	~World();

protected:
	//Frame loop
	void FrameStart();
	void Update();
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
	void hCharacterCreate(CharacterPacket* const);
	void hCharacterDelete(CharacterPacket* const);
	void hQueryCharacterExists(CharacterPacket* const);
	void hQueryCharacterStats(CharacterPacket* const);
	void hQueryCharacterLocation(CharacterPacket* const);
	void hCharacterMovement(CharacterPacket* const);
	void hCharacterAttack(CharacterPacket* const);
	void hCharacterDamage(CharacterPacket* const);

	//monster management
	void hMonsterCreate(MonsterPacket* const);
	void hMonsterDelete(MonsterPacket* const);
	void hQueryMonsterExists(MonsterPacket* const);
	void hQueryMonsterStats(MonsterPacket* const);
	void hQueryMonsterLocation(MonsterPacket* const);
	void hMonsterMovement(MonsterPacket* const);
	void hMonsterAttack(MonsterPacket* const);
	void hMonsterDamage(MonsterPacket* const);

	//chat
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
	Image buttonImage;
	RasterFont font;
	TileSheet tileSheet;

	//map
	RegionPagerBase regionPager;

	//UI
	Button disconnectButton;
	Button shutDownButton;
	FrameRate fps;

	//the camera structure
	struct {
		int x = 0, y = 0;
		int width = 0, height = 0;
		int marginX = 0, marginY = 0;
	} camera;

	//entities
	std::map<int, BaseCharacter> characterMap;
	std::map<int, BaseMonster> monsterMap;
	LocalCharacter* localCharacter = nullptr;

	//heartbeat
	//TODO: (9) Heartbeat needs it's own utility
	typedef std::chrono::steady_clock Clock;
	Clock::time_point lastBeat = Clock::now();
	int attemptedBeats = 0;

	//ugly references; I hate this
	ConfigUtility& config = ConfigUtility::GetSingleton();
	UDPNetworkUtility& network = UDPNetworkUtility::GetSingleton();
};

#endif
