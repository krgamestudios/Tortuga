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
#ifndef CLIENTAPPLICATION_HPP_
#define CLIENTAPPLICATION_HPP_

#include "scene_list.hpp"
#include "base_scene.hpp"

#include "config_utility.hpp"
#include "network_packet.hpp"
#include "udp_network_utility.hpp"

class ClientApplication {
private:
	ClientApplication();
	~ClientApplication();
	static ClientApplication instance;

public:
	static ClientApplication* GetInstance() { return &instance; }

	void Init();
	void Proc();
	void Quit();

private:
	//Private access members
	void LoadScene(SceneList sceneIndex);
	void UnloadScene();

	BaseScene* activeScene = nullptr;

	ConfigUtility config;
	UDPNetworkUtility network;
};

#endif
