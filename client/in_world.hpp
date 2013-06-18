#ifndef INWORLD_HPP_
#define INWORLD_HPP_

#include "base_scene.hpp"
#include "defines.hpp"
#include "service_locator.hpp"
#include "packet_type.hpp"
#include "network_queue.hpp"
#include "information_manager.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "udp_network_utility.hpp"
#include "button.hpp"
#include "raster_font.hpp"

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
	int HandlePacket(Packet p);
	void Disconnect();
	void ExitGame();
	void HandleDisconnection(::Disconnect&);

	//services
	ConfigUtility* configUtil = ServiceLocator<ConfigUtility>::Get();
	SurfaceManager* surfaceMgr = ServiceLocator<SurfaceManager>::Get();
	UDPNetworkUtility* netUtil = ServiceLocator<UDPNetworkUtility>::Get();
	InformationManager* infoMgr = ServiceLocator<InformationManager>::Get();

	//members
	//...
};

#endif
