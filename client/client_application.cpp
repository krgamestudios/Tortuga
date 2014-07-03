/* Copyright: (c) Kayne Ruse 2013, 2014
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial ClientApplications, and to alter it and redistribute it
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
#include "client_application.hpp"

#include "serial.hpp"

#include <stdexcept>
#include <chrono>
#include <iostream>

//-------------------------
//Scene headers
//-------------------------

//Add the custom scene headers here
#include "splash_screen.hpp"
#include "main_menu.hpp"
#include "options_menu.hpp"
#include "lobby_menu.hpp"
#include "in_world.hpp"
#include "in_combat.hpp"
#include "clean_up.hpp"

//-------------------------
//Public access members
//-------------------------

void ClientApplication::Init(int argc, char** argv) {
	std::cout << "Beginning " << argv[0] << std::endl;

	//load the prerequisites
	config.Load("rsc\\config.cfg");

	//-------------------------
	//Initialize the APIs
	//-------------------------

	//initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO)) {
		throw(std::runtime_error("Failed to initialize SDL"));
	}
	std::cout << "Initialized SDL" << std::endl;

	//initialize SDL_net
	if (SDLNet_Init()) {
		throw(std::runtime_error("Failed to initialize SDL_net"));
	}
	network.Open(0);
	std::cout << "Initialized SDL_net" << std::endl;

	//-------------------------
	//Setup the screen
	//-------------------------

	int w = config.Int("client.screen.w");
	int h = config.Int("client.screen.h");
	int f = config.Bool("client.screen.f") ? SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN : SDL_HWSURFACE|SDL_DOUBLEBUF;

	BaseScene::SetScreen(w ? w : 800, h ? h : 600, 0, f);
	std::cout << "Initialized the screen" << std::endl;

	//-------------------------
	//debug output
	//-------------------------

	//TODO: enable/disable these with a switch
#define DEBUG_OUTPUT_VAR(x) std::cout << "\t" << #x << ": " << x << std::endl;

	std::cout << "Internal sizes:" << std::endl;

	DEBUG_OUTPUT_VAR(sizeof(Region::type_t));
	DEBUG_OUTPUT_VAR(sizeof(Region));
	DEBUG_OUTPUT_VAR(REGION_WIDTH);
	DEBUG_OUTPUT_VAR(REGION_HEIGHT);
	DEBUG_OUTPUT_VAR(REGION_DEPTH);
	DEBUG_OUTPUT_VAR(REGION_SOLID_FOOTPRINT);
	DEBUG_OUTPUT_VAR(REGION_FOOTPRINT);
	DEBUG_OUTPUT_VAR(PACKET_BUFFER_SIZE);
	DEBUG_OUTPUT_VAR(MAX_PACKET_SIZE);

#undef DEBUG_OUTPUT_VAR

	//-------------------------
	//finalize the startup
	//-------------------------

	std::cout << "Startup completed successfully" << std::endl;

	//-------------------------
	//debugging
	//-------------------------

	//...
}

void ClientApplication::Proc() {
	LoadScene(SceneList::FIRST);

	//prepare the time system
	typedef std::chrono::steady_clock Clock;

	std::chrono::duration<int, std::milli> delta(16);
	Clock::time_point simTime = Clock::now();
	Clock::time_point realTime;

	//The main loop
	while(activeScene->GetNextScene() != SceneList::QUIT) {
		//switch scenes when necessary
		if (activeScene->GetNextScene() != SceneList::CONTINUE) {
			LoadScene(activeScene->GetNextScene());
			continue;
		}

		//update the current time
		realTime = Clock::now();

		//simulate game time
		while (simTime < realTime) {
			//call each user defined function
			activeScene->RunFrame(double(delta.count()) / std::chrono::duration<int, std::milli>::period::den);
			simTime += delta;
		}

		//draw the game to the screen
		activeScene->RenderFrame();
	}

	UnloadScene();
}

void ClientApplication::Quit() {
	std::cout << "Shutting down" << std::endl;
	network.Close();
	SDLNet_Quit();
	SDL_Quit();
	std::cout << "Clean exit" << std::endl;
}

//-------------------------
//Private access members
//-------------------------

void ClientApplication::LoadScene(SceneList sceneIndex) {
	UnloadScene();
	switch(sceneIndex) {
		//add scene creation calls here
		case SceneList::FIRST:
		case SceneList::SPLASHSCREEN:
			activeScene = new SplashScreen(&config);
		break;
		case SceneList::MAINMENU:
			activeScene = new MainMenu(&config);
		break;
		case SceneList::OPTIONSMENU:
			activeScene = new OptionsMenu(&config);
		break;
		case SceneList::LOBBYMENU:
			activeScene = new LobbyMenu(&config, &network, &clientIndex, &accountIndex);
		break;
		case SceneList::INWORLD:
			activeScene = new InWorld(&config, &network, &clientIndex, &accountIndex, &characterIndex, &characterMap);
		break;
		case SceneList::INCOMBAT:
			activeScene = new InCombat(&config, &network, &clientIndex, &accountIndex, &characterIndex, &characterMap);
		break;
		case SceneList::CLEANUP:
			activeScene = new CleanUp(&config, &network, &clientIndex, &accountIndex, &characterIndex, &characterMap);
		break;
		default:
			throw(std::logic_error("Failed to recognize the scene index"));
	}
}

void ClientApplication::UnloadScene() {
	delete activeScene;
	activeScene = nullptr;
}
