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

#include "timer.hpp"

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
#include "in_world.hpp"

//-------------------------
//Public access members
//-------------------------

void Application::Init(int argc, char** argv) {
	std::cout << "Beginning " << argv[0] << std::endl;

	//-------------------------
	//Initialize the APIs
	//-------------------------

	//initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO)) {
		throw(std::runtime_error("Failed to initialize SDL"));
	}
	std::cout << "Initialized SDL" << std::endl;

	//initialize lua
	lua = luaL_newstate();
	if (!lua) {
		throw(std::runtime_error("Failed to initialize lua"));
	}
	luaL_openlibs(lua);
	std::cout << "Initialized lua" << std::endl;

	if (luaL_dofile(lua, "rsc\\setup.lua")) {
		throw(std::runtime_error("Failed to initialize lua's startup script"));
	}
	std::cout << "Initialized lua's setup script" << std::endl;

	//-------------------------
	//Setup the screen
	//-------------------------

	lua_getglobal(lua, "config");
	lua_getfield(lua, 1, "screen");
	lua_getfield(lua, 2, "width");
	lua_getfield(lua, 2, "height");
	lua_getfield(lua, 2, "fullscreen");

	int w = lua_tointeger(lua, 3);
	int h = lua_tointeger(lua, 4);
	int f = lua_toboolean(lua, 5);

	lua_pop(lua, 5);

	BaseScene::SetScreen(w ? w : 800, h ? h : 600, 0, f ? SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN : SDL_HWSURFACE|SDL_DOUBLEBUF);
	std::cout << "Initialized the screen" << std::endl;

	//-------------------------
	//finalize the startup
	//-------------------------

	std::cout << "Startup completed successfully" << std::endl;

	//-------------------------
	//debugging
	//-------------------------

	//...
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

		Timer runTimer("run");

		//simulate game time
		while (simTime < realTime) {
			//call each user defined function
			activeScene->RunFrame(double(delta.count()) / std::chrono::duration<int, std::milli>::period::den);
			simTime += delta;
		}

		runTimer.Stop();

		Timer renderTimer("render");

		//draw the game to the screen
		activeScene->RenderFrame();

		renderTimer.Stop();

		//debugging output
//		std::cout << runTimer << std::endl;
//		std::cout << renderTimer << std::endl;
	}

	UnloadScene();
}

void Application::Quit() {
	std::cout << "Shutting down" << std::endl;
	lua_close(lua);
	SDL_Quit();
	std::cout << "Clean exit" << std::endl;
}

//-------------------------
//Private access members
//-------------------------

void Application::LoadScene(SceneList sceneIndex) {
	UnloadScene();
	switch(sceneIndex) {
		//add scene creation calls here
		case SceneList::FIRST:
		case SceneList::SPLASHSCREEN:
			activeScene = new SplashScreen(lua);
		break;
		case SceneList::MAINMENU:
			activeScene = new MainMenu(lua);
		break;
		case SceneList::OPTIONSMENU:
			activeScene = new OptionsMenu(lua);
		break;
		case SceneList::INWORLD:
			activeScene = new InWorld(lua);
		break;
		default:
			throw(std::logic_error("Failed to recognize the scene index"));
	}
}

void Application::UnloadScene() {
	delete activeScene;
	activeScene = nullptr;
}
