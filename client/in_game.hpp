#ifndef INGAME_HPP_
#define INGAME_HPP_

#include "base_scene.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "udp_network_utility.hpp"
#include "packet_list.hpp"

class InGame : public BaseScene {
public:
	//Public access members
	InGame(ConfigUtility*, SurfaceManager*, UDPNetworkUtility*, int* playerID);
	~InGame();

protected:
	//Frame loop
	void FrameStart();
	void Update(double delta);
	void Receive();
	void FrameEnd();
	void Render(SDL_Surface* const);

	//Event handlers
	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	void MouseButtonUp(SDL_MouseButtonEvent const&);
	void KeyDown(SDL_KeyboardEvent const&);
	void KeyUp(SDL_KeyboardEvent const&);

	//members
	ConfigUtility* configUtil = nullptr;
	SurfaceManager* surfaceMgr = nullptr;
	UDPNetworkUtility* netUtil = nullptr;
	int* playerID = nullptr;
};

#endif
