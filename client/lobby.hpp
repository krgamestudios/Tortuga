#ifndef LOBBY_HPP_
#define LOBBY_HPP_

#include "base_scene.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "udp_network_utility.hpp"
#include "packet_list.hpp"

#include "raster_font.hpp"
#include "button.hpp"

#include <vector>
#include <map>
#include <string>

class Lobby : public BaseScene {
public:
	//Public access members
	Lobby(ConfigUtility*, SurfaceManager*, UDPNetworkUtility*);
	virtual ~Lobby();

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

	//utilities
	struct ServerData {
		std::string name;
		IPaddress address;
	};

	void PingNetwork();
	void PushServer(PacketData*);
	void JoinRequest(ServerData*);

	//members
	ConfigUtility* configUtil = nullptr;
	SurfaceManager* surfaceMgr = nullptr;
	UDPNetworkUtility* netUtil = nullptr;

	RasterFont font;
	std::map<std::string, Button*> buttonMap;

	//the list of servers on the screen
	std::vector<ServerData> serverVector;
	ServerData* selectedServer = nullptr;
	SDL_Rect listBox;
};

#endif
