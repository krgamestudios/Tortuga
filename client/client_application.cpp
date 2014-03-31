/* Copyright: (c) Kayne Ruse 2013
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

#include <stdexcept>
#include <chrono>

//-------------------------
//Static declarations
//-------------------------

ClientApplication ClientApplication::instance;

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

//-------------------------
//Public access members
//-------------------------

ClientApplication::ClientApplication() {
	//
}

ClientApplication::~ClientApplication() {
	//
}

void ClientApplication::Init() {
	//load the prerequisites
	config.Load("rsc\\config.cfg");

	//initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO)) {
		throw(std::runtime_error("Failed to initialize SDL"));
	}
	BaseScene::SetScreen(config.Int("screen.w"), config.Int("screen.h"), 0, (config.Bool("screen.f")) ? SDL_HWSURFACE|SDL_DOUBLEBUF : SDL_HWSURFACE);

	//initialize SDL_net
	if (SDLNet_Init()) {
		throw(std::runtime_error("Failed to initialize SDL_net"));
	}
	network.Open(0, PACKET_BUFFER_SIZE);
}

void ClientApplication::Proc() {
	LoadScene(SceneList::FIRST);

	//prepare the time system
	typedef std::chrono::steady_clock Clock;

	Clock::duration delta(16 * Clock::duration::period::den / std::milli::den);
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
			activeScene->RunFrame(double(delta.count()) / Clock::duration::period::den);
			simTime += delta;
		}

		//draw the game to the screen
		activeScene->RenderFrame();

		//give the computer a break
		SDL_Delay(10);
	}

	UnloadScene();
}

void ClientApplication::Quit() {
	network.Close();
	SDLNet_Quit();
	SDL_Quit();
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
			activeScene = new LobbyMenu(&config, &network, &clientIndex);
		break;
		case SceneList::INWORLD:
			activeScene = new InWorld(&config, &network, &clientIndex);
		break;
		case SceneList::INCOMBAT:
			activeScene = new InCombat();
		break;

		default:
			throw(std::logic_error("Failed to recognize the scene index"));
	}
}

void ClientApplication::UnloadScene() {
	delete activeScene;
	activeScene = nullptr;
}
