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
#ifndef LOBBYMENU_HPP_
#define LOBBYMENU_HPP_

#include "base_scene.hpp"

#include "image.hpp"
#include "raster_font.hpp"
#include "button.hpp"

#include "config_utility.hpp"
#include "udp_network_utility.hpp"
#include "network_packet.hpp"

#include <vector>

class LobbyMenu : public BaseScene {
public:
	//Public access members
	LobbyMenu(ConfigUtility* const, UDPNetworkUtility* const, int* const);
	~LobbyMenu();

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

	void HandlePacket(NetworkPacket);

	//global
	ConfigUtility& config;
	UDPNetworkUtility& network;
	int& clientIndex;

	//members
	Image image;
	RasterFont font;
	Button search;
	Button join;
	Button back;

	//server list
	struct ServerInformation {
		std::string name;
		IPaddress address;
	};

	std::vector<ServerInformation> serverInfo;

	//a terrible hack, forgive me
	//I'd love a proper gui system for this
	SDL_Rect listBox;
	ServerInformation* selection = nullptr;
};

#endif
