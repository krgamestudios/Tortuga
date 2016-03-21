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

#include "base_scene.hpp"
#include "scene_signal.hpp"
#include "singleton.hpp"
#include "udp_network_utility.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#include "SDL2/SDL_ttf.h"

class ClientApplication: public Singleton<ClientApplication> {
public:
	void Init(int argc, char* argv[]);
	void Proc();
	void Quit();

private:
	friend Singleton<ClientApplication>;

	ClientApplication() = default;
	~ClientApplication() = default;

	//scene management
	void ProcessEvents();
	void ProcessSceneSignal(SceneSignal);
	void ClearScene();

	BaseScene* activeScene = nullptr;

	//TODO: (9) build a "window" class?
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	//global scene parameters
	int clientIndex = -1;
	int accountIndex = -1;
};