#include "scene_manager.h"

#include <stdexcept>

//-------------------------
//Scene headers
//-------------------------

//Add the custom scene headers here
#include "test_systems.h"
#include "in_game.h"

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

	BaseScene::SetScreen(800, 600);
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
		case SceneList::FIRST:
		case SceneList::INGAME:
			activeScene = new InGame();
		break;
		case SceneList::TESTSYSTEMS:
			activeScene = new TestSystems();
		break;

		default:
			throw(std::logic_error("Failed to recognize the scene index"));
	}
}

void SceneManager::UnloadScene() {
	delete activeScene;
	activeScene = nullptr;
}
