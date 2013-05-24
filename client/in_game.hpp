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
	virtual ~InGame();

protected:
	//Frame loop
	virtual void FrameStart();
	virtual void FrameEnd();
	virtual void Update();
	virtual void Receive();
	virtual void Render(SDL_Surface* const);

	//Event handlers
	virtual void MouseMotion(SDL_MouseMotionEvent const&);
	virtual void MouseButtonDown(SDL_MouseButtonEvent const&);
	virtual void MouseButtonUp(SDL_MouseButtonEvent const&);
	virtual void KeyDown(SDL_KeyboardEvent const&);
	virtual void KeyUp(SDL_KeyboardEvent const&);

	//members
	ConfigUtility* configUtil = nullptr;
	SurfaceManager* surfaceMgr = nullptr;
	UDPNetworkUtility* netUtil = nullptr;
	int* playerID;
};

#endif
