#ifndef LOBBY_HPP_
#define LOBBY_HPP_

#include "base_scene.hpp"
#include "service_locator.hpp"
#include "packet_type.hpp"
#include "network_queue.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "udp_network_utility.hpp"
#include "button.hpp"
#include "raster_font.hpp"

#include "SDL/SDL_thread.h"

#include <vector>
#include <string>

struct ServerEntry {
	std::string name;
	IPaddress address;
};

class Lobby : public BaseScene {
public:
	//Public access members
	Lobby();
	~Lobby();

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
	int HandlePacket(Packet p);
	void BroadcastNetwork();
	void PushServer(BroadcastResponse&);

	//services
	ConfigUtility* configUtil = ServiceLocator<ConfigUtility>::Get();
	SurfaceManager* surfaceMgr = ServiceLocator<SurfaceManager>::Get();
	UDPNetworkUtility* netUtil = ServiceLocator<UDPNetworkUtility>::Get();

	//members
	Button refreshButton;
	Button joinButton;
	Button backButton;

	RasterFont font;
	SDL_Rect listBox;
	std::vector<ServerEntry> serverList;
	ServerEntry* selectedServer = nullptr;

	//threads
	SDL_Thread* queueThread = nullptr;
};

#endif
