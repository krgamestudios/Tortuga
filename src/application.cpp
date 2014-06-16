/* Copyright: (c) Kayne Ruse 2013, 2014
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial Applications, and to alter it and redistribute it
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
#include "application.hpp"

#include <stdexcept>
#include <chrono>
#include <sstream>

//-------------------------
//Scene headers
//-------------------------

//Add the custom scene headers here
#include "shell_scene.hpp"

//-------------------------
//Public access members
//-------------------------

void Application::Init(int argc, char** argv) {
	//initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO)) {
		throw(std::runtime_error("Failed to initialize SDL"));
	}
	BaseScene::SetScreen(800, 600, 0, SDL_HWSURFACE|SDL_DOUBLEBUF);
}

void Application::Proc() {
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

void Application::Quit() {
	SDL_Quit();
}

//-------------------------
//Private access members
//-------------------------

void Application::LoadScene(SceneList sceneIndex) {
	UnloadScene();
	switch(sceneIndex) {
		//add scene creation calls here
		case SceneList::FIRST:
		case SceneList::SHELL:
			activeScene = new ShellScene();
		break;
		default: {
			std::ostringstream msg;
			msg << "Failed to recognize the scene index: ";
			msg << sceneIndex;
			throw(std::logic_error(msg.str()));
		}
	}
}

void Application::UnloadScene() {
	delete activeScene;
	activeScene = nullptr;
}
