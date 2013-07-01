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
#include "client_application.hpp"

#include <stdexcept>
#include <chrono>

//-------------------------
//Scene headers
//-------------------------

//Add the custom scene headers here
#include "in_combat.hpp"
#include "in_world.hpp"
#include "lobby.hpp"
#include "main_menu.hpp"
#include "option_screen.hpp"
#include "splash_screen.hpp"

//-------------------------
//Public access members
//-------------------------

ClientApplication::ClientApplication() {
	//
}

ClientApplication::~ClientApplication() {
	UnloadScene();
}

/* ClientApplication::Init()
 * This function initializes the entire program. There are a number of things
 * that could go wrong here, which is why there is such an unusual order of
 * operations.
 * Important things to note:
 *   The APIs are initiated here.
 *   The global objects are initialized here.
 *   The game's screen is created here, based on information loaded from the config file.
 *   The ConfigUtility's call to Load() also ensures that the "rsc\" folder is in the directory. It's easy to forget it.
*/

void ClientApplication::Init() {
	//load the config file
	try {
		configUtil->Load("rsc/config.cfg");
	}
	catch(std::runtime_error& e) {
		std::string s = e.what();
		s += "; Ensure that the \"rsc\" directory is present";
		throw(std::runtime_error(s));
	}

	//initialize the APIs
	if (SDL_Init(SDL_INIT_VIDEO)) {
		throw(std::runtime_error("Failed to initialize SDL"));
	}
	if (SDLNet_Init()) {
		throw(std::runtime_error("Failed to initialize SDL_net"));
	}

	//create the screen
	Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
	flags |= configUtil->Bool("screen.f") ? SDL_FULLSCREEN : 0;

	BaseScene::SetScreen(
		configUtil->Int("screen.w"),
		configUtil->Int("screen.h"),
		SDL_GetVideoInfo()->vfmt->BitsPerPixel,
		flags);

	//initiate the remaining singletons
	netUtil->Open(0, sizeof(Packet));
}

void ClientApplication::Proc() {
	LoadScene(SceneList::FIRST);

	//prepare the time system
	typedef std::chrono::high_resolution_clock Clock;

	Clock::duration delta(5 * Clock::duration::period::den / std::milli::den);
	Clock::time_point simTime = Clock::now();
	Clock::time_point realTime;

	//The main loop
	while(activeScene->GetNextScene() != SceneList::QUIT) {
		//switch scenes when necessary
		if (activeScene->GetNextScene() != SceneList::CONTINUE) {
			LoadScene(activeScene->GetNextScene());
			continue;
		}

		//update the current time & give the computer as much of a break as possible
		while ((realTime = Clock::now()) < simTime) {
			SDL_Delay(1);
		}

		//simulate game time
		while (simTime < realTime) {
			//call each user defined function
			activeScene->RunFrame(double(delta.count()) / Clock::duration::period::den);
			simTime += delta;
		}

		//draw the game to the screen
		activeScene->RenderFrame();
	}

	UnloadScene();
}

void ClientApplication::Quit() {
	//clean up the singletons
	netUtil->Close();
	surfaceMgr->FreeAll();

	//clean up the scene
	UnloadScene();

	//deinitialize the APIs
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
		case SceneList::INCOMBAT:
			activeScene = new InCombat();
		break;
		case SceneList::INWORLD:
			activeScene = new InWorld();
		break;
		case SceneList::LOBBY:
			activeScene = new Lobby();
		break;
		case SceneList::MAINMENU:
			activeScene = new MainMenu();
		break;
		case SceneList::OPTIONSCREEN:
			activeScene = new OptionScreen();
		break;
		case SceneList::FIRST:
		case SceneList::SPLASHSCREEN:
			activeScene = new SplashScreen();
		break;

		default:
			throw(std::logic_error("Failed to recognize the scene index"));
	}
}

void ClientApplication::UnloadScene() {
	delete activeScene;
	activeScene = nullptr;
}
