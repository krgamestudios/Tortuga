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
	virtual ~TestSystems();

protected:
	//Frame loop
	virtual void FrameStart();
	virtual void FrameEnd();
	virtual void Update();
	virtual void Render(SDL_Surface* const);

	//Event handlers
	virtual void MouseMotion(SDL_MouseMotionEvent const&);
	virtual void MouseButtonDown(SDL_MouseButtonEvent const&);
	virtual void MouseButtonUp(SDL_MouseButtonEvent const&);
	virtual void KeyDown(SDL_KeyboardEvent const&);
	virtual void KeyUp(SDL_KeyboardEvent const&);

	//utilities
	void NewPlayer(int index, std::string avatarName, int x, int y);
	void SwitchToPlayer(int index);
	void SendMessage(std::string);

	//members
	ConfigUtility* configUtil = nullptr;
	SurfaceManager* surfaceMgr = nullptr;
	UDPNetworkUtility* netUtil = nullptr;

	PlayerManager playerMgr;

	FrameRate frameRate;
	RasterFont font;

	int playerCounter;
	int currentPlayer;
};

#endif
