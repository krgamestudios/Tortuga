#include "scene_manager.hpp"

#include <stdexcept>
#include <chrono>

//-------------------------
//Scene headers
//-------------------------

//Add the custom scene headers here
#include "test_systems.hpp"

#include "splash.hpp"
#include "main_menu.hpp"
#include "lobby.hpp"
#include "in_game.hpp"

//-------------------------
//Public access members
//-------------------------

SceneManager::SceneManager() {
	//
}

SceneManager::~SceneManager() {
	UnloadScene();
}

void SceneManager::Init() {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		throw(std::runtime_error("Failed to initialize SDL"));
	}
	if (SDLNet_Init()) {
		throw(std::runtime_error("Failed to initialize SDL_net"));
	}

	configUtil.Load("rsc/config.cfg");
	netUtil.Open(0, sizeof(PacketData));

	//set the screen from the config file
	int flags = SDL_HWSURFACE|SDL_DOUBLEBUF;
	if (configUtil.Boolean("screen.f")) {
		flags |= SDL_FULLSCREEN;
	}
	BaseScene::SetScreen(configUtil.Integer("screen.w"),configUtil.Integer("screen.h"),0,flags);
}

void SceneManager::Proc() {
	LoadScene(SceneList::FIRST);

	//prepare the time system
	typedef std::chrono::high_resolution_clock Clock;

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

void SceneManager::Quit() {
	UnloadScene();
	SDLNet_Quit();
	SDL_Quit();
}

//-------------------------
//Private access members
//-------------------------

void SceneManager::LoadScene(SceneList sceneIndex) {
	UnloadScene();

	switch(sceneIndex) {
		//add scene creation calls here
		case SceneList::TESTSYSTEMS:
			activeScene = new TestSystems(&configUtil, &surfaceMgr, &netUtil);
		break;

		case SceneList::FIRST:
		case SceneList::SPLASH:
			activeScene = new Splash(&configUtil, &surfaceMgr);
		break;
		case SceneList::MAINMENU:
			activeScene = new MainMenu(&configUtil, &surfaceMgr);
		break;
		case SceneList::LOBBY:
			activeScene = new Lobby(&configUtil, &surfaceMgr, &netUtil, &playerID);
		break;
		case SceneList::INGAME:
			activeScene = new InGame(&configUtil, &surfaceMgr, &netUtil, &playerID);
		break;

		default:
			throw(std::logic_error("Failed to recognize the scene index"));
	}
}

void SceneManager::UnloadScene() {
	delete activeScene;
	activeScene = nullptr;
}
