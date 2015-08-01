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

//graphics & ui
#include "image.hpp"
#include "button.hpp"
#include "bounding_box.hpp"

//utilities
#include "config_utility.hpp"
#include "udp_network_utility.hpp"
#include "serial_packet.hpp"

//client
#include "base_scene.hpp"

//STL
#include <vector>

class LobbyMenu : public BaseScene {
public:
	//Public access members
	LobbyMenu(int* const argClientIndex, int* const argAccountIndex);
	~LobbyMenu();

	void RenderFrame(SDL_Renderer* renderer) override;

protected:
	//frame phases
	void FrameStart() override;
	void Update() override;
	void FrameEnd() override;

	//input events
	void MouseMotion(SDL_MouseMotionEvent const& event) override;
	void MouseButtonDown(SDL_MouseButtonEvent const& event) override;
	void MouseButtonUp(SDL_MouseButtonEvent const& event) override;
	void MouseWheel(SDL_MouseWheelEvent const& event) override;
	void KeyDown(SDL_KeyboardEvent const& event) override;
	void KeyUp(SDL_KeyboardEvent const& event) override;

	//Network handlers
	void HandlePacket(SerialPacket* const);
	void HandleBroadcastResponse(ServerPacket* const);
	void HandleJoinResponse(ClientPacket* const);
	void HandleLoginResponse(ClientPacket* const);
	void HandleJoinRejection(TextPacket* const);
	void HandleLoginRejection(TextPacket* const);

	//server control
	void SendBroadcastRequest();
	void SendJoinRequest();
	void SendLoginRequest();

	//shared parameters
	ConfigUtility& config = ConfigUtility::GetSingleton();
	UDPNetworkUtility& network = UDPNetworkUtility::GetSingleton();
	int& clientIndex;
	int& accountIndex;

	//members
	Image image;
	Button search;
	Button join;
	Button back;

	//server list
	struct ServerInformation {
		IPaddress address;
		std::string name;
		int playerCount;
		int version;
		bool compatible;
	};

	std::vector<ServerInformation> serverInfo;

	//NOTE: a terrible hack
	//I'd love a proper gui system for this
	BoundingBox listBox;
	ServerInformation* selection = nullptr;
};
