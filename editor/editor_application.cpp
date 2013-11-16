/* Copyright: (c) Kayne Ruse 2013
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial EditorApplications, and to alter it and redistribute it
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
#include "editor_application.hpp"

#include <stdexcept>
#include <chrono>

//-------------------------
//Static declarations
//-------------------------

EditorApplication EditorApplication::instance;

//-------------------------
//Scene headers
//-------------------------

//Add the custom scene headers here
#include "editor_scene.hpp"
#include "testificate_scene.hpp"

//-------------------------
//Public access members
//-------------------------

EditorApplication::EditorApplication() {
	//
}

EditorApplication::~EditorApplication() {
	//
}

void EditorApplication::Init() {
	config.Load("rsc\\config.cfg");
	if (SDL_Init(SDL_INIT_VIDEO))
		throw(std::runtime_error("Failed to initialize SDL"));

	BaseScene::SetScreen(config.Int("screen.w"), config.Int("screen.h"));
}

void EditorApplication::Proc() {
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

void EditorApplication::Quit() {
	SDL_Quit();
}

//-------------------------
//Private access members
//-------------------------

void EditorApplication::LoadScene(SceneList sceneIndex) {
	UnloadScene();

	switch(sceneIndex) {
		//add scene creation calls here
		case SceneList::FIRST:
		case SceneList::EDITORSCENE:
			activeScene = new EditorScene(&config);
		break;

		case SceneList::TESTIFICATESCENE:
			activeScene = new TestificateScene(&config);
		break;


		default:
			throw(std::logic_error("Failed to recognize the scene index"));
	}
}

void EditorApplication::UnloadScene() {
	delete activeScene;
	activeScene = nullptr;
}
