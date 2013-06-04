#ifndef TESTSYSTEMS_HPP_
#define TESTSYSTEMS_HPP_

#include "base_scene.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "udp_network_utility.hpp"
#include "packet_list.hpp"

#include "player_manager.hpp"
#include "frame_rate.hpp"
#include "raster_font.hpp"

#include <string>

class TestSystems : public BaseScene {
public:
	//Public access members
	TestSystems(ConfigUtility*, SurfaceManager*, UDPNetworkUtility*);
	~TestSystems();

protected:
	//Frame loop
	void FrameStart();
	void Update(double delta);
	void FrameEnd();
	void Render(SDL_Surface* const);

	//Event handlers
	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	void MouseButtonUp(SDL_MouseButtonEvent const&);
	void KeyDown(SDL_KeyboardEvent const&);
	void KeyUp(SDL_KeyboardEvent const&);

	//utilities
	void NewPlayer(int index, std::string avatarName, int x, int y);
	void SwitchToPlayer(int index);
	void SendMessage(std::string);

	//members
	ConfigUtility* configUtil = nullptr;
	SurfaceManager* surfaceMgr = nullptr;
	UDPNetworkUtility* netUtil = nullptr;

	PlayerManager playerMgr;
	RasterFont font;

	int playerCounter;
	int currentPlayer;
};

#endif
