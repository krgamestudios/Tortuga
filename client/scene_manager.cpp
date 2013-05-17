#include "scene_manager.hpp"

#include <stdexcept>

//-------------------------
//Scene headers
//-------------------------

//Add the custom scene headers here
#ifdef DEBUG
#include "test_systems.hpp"
#endif

#include "splash.hpp"
#include "main_menu.hpp"
#include "in_game.hpp"

#ifdef DEBUG
#include "lobby.hpp"
#include "combat.hpp"
#endif

//-------------------------
//Public access members
//-------------------------

SceneManager::SceneManager() {
	activeScene = nullptr;
}

SceneManager::~SceneManager() {
	UnloadScene();
}

void SceneManager::Init() {
	if (SDL_Init(SDL_INIT_VIDEO))
		throw(std::runtime_error("Failed to initialize SDL"));

	configUtil.Load("rsc/config.cfg");

	//set the screen from the config file
	int flags = SDL_HWSURFACE|SDL_DOUBLEBUF;
	if (configUtil.Boolean("screen.f")) {
		flags |= SDL_FULLSCREEN;
	}
	BaseScene::SetScreen(configUtil.Integer("screen.w"),configUtil.Integer("screen.h"),0,flags);
}

void SceneManager::Proc() {
	LoadScene(SceneList::FIRST);

	//The main loop
	while(activeScene->GetNextScene() != SceneList::QUIT) {
		//switch scenes when necessary
		if (activeScene->GetNextScene() != SceneList::CONTINUE) {
			LoadScene(activeScene->GetNextScene());
			continue;
		}

		//call each user defined function
		activeScene->RunFrame();
	}

	UnloadScene();
}

void SceneManager::Quit() {
	UnloadScene();
	SDL_Quit();
}

//-------------------------
//Private access members
//-------------------------

void SceneManager::LoadScene(SceneList sceneIndex) {
	UnloadScene();

	switch(sceneIndex) {
		//add scene creation calls here
#ifdef DEBUG
		case SceneList::TESTSYSTEMS:
			activeScene = new TestSystems(&configUtil, &surfaceMgr);
		break;
#endif

		case SceneList::FIRST:
		case SceneList::SPLASH:
			activeScene = new Splash(&configUtil, &surfaceMgr);
		break;
		case SceneList::MAINMENU:
			activeScene = new MainMenu(&configUtil, &surfaceMgr);
		break;
		case SceneList::INGAME:
			activeScene = new InGame(&configUtil, &surfaceMgr);
		break;
		case SceneList::LOBBY:
			activeScene = new Lobby(&configUtil, &surfaceMgr);
		break;

#ifdef DEBUG
		case SceneList::COMBAT:
			activeScene = new Combat();
		break;
#endif

		default:
			throw(std::logic_error("Failed to recognize the scene index"));
	}
}

void SceneManager::UnloadScene() {
	delete activeScene;
	activeScene = nullptr;
}
