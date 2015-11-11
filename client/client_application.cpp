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
#include "client_application.hpp"

#include "serial_packet.hpp"
#include "config_utility.hpp"

//for handling platforms
#include "SDL2/SDL_syswm.h"
#include "SDL2/SDL_version.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdexcept>

//-------------------------
//Public access members
//-------------------------

void ClientApplication::Init(int argc, char* argv[]) {
	std::cout << "Beginning " << argv[0] << std::endl;

	//load the prerequisites
	ConfigUtility& config = ConfigUtility::GetSingleton();
	config.Load("rsc/config.cfg", false, argc, argv);

	//-------------------------
	//Initialize SDL
	//-------------------------

	if (SDL_Init(0)) {
		std::ostringstream msg;
		msg << "Failed to initialize SDL: " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	//-------------------------
	//create and check the window
	//-------------------------

	//get the config values
	int w = config.Int("client.screen.w");
	int h = config.Int("client.screen.h");
	int f = config.Boolean("client.screen.f") ? SDL_WINDOW_FULLSCREEN : 0;

	//BUG: fullscreen is disabled
	f = 0;

	//default sizes
	w = w ? w : 800;
	h = h ? h : 600;

	window = SDL_CreateWindow(argv[0], SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, f);

	if (!window) {
		std::ostringstream msg;
		msg << "Failed to create the window: " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	std::cout << "Initialized the window" << std::endl;

	//-------------------------
	//DEBUG: detecting platforms, versions & appropriate fonts
	//-------------------------

	SDL_SysWMinfo windowInfo;
	SDL_VERSION(&windowInfo.version);
	if (SDL_GetWindowWMInfo(window, &windowInfo)) {
		//
		std::string platform;
		std::string fontPath;

		//get the info
		switch(windowInfo.subsystem) {
			case SDL_SYSWM_WINDOWS:
				platform = "Microsoft Windows";
			break;

			case SDL_SYSWM_X11:
				platform = "X Window System";
			break;

			//NOTE: OS X is currently unsupported, but it could be
			case SDL_SYSWM_COCOA:
				platform = "Apple OS X";
			break;

			default:
				platform = "an unsupported platform";
		}

		//final output
		std::cout << "SDL Version ";
		std::cout << (int)windowInfo.version.major << ".";
		std::cout << (int)windowInfo.version.minor << ".";
		std::cout << (int)windowInfo.version.patch << " on ";
		std::cout << platform << std::endl;
	}
	else {
		std::ostringstream msg;
		msg << "Failed to retrieve window info: " << SDL_GetError();
		throw(msg.str());
	}

	//-------------------------
	//create and check the renderer
	//-------------------------

	renderer = SDL_CreateRenderer(window, -1, 0);

	if (!renderer) {
		std::ostringstream msg;
		msg << "Failed to create the renderer: " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	//screen scaling
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	SDL_RenderSetLogicalSize(renderer, w, h);

	//set the hook for the renderer
	BaseScene::SetRenderer(renderer);

	std::cout << "Initialized the renderer" << std::endl;

	//-------------------------
	//Initialize the APIs
	//-------------------------

	//initialize SDL_net
	if (SDLNet_Init()) {
		std::ostringstream msg;
		msg << "Failed to initialize SDL_net 2.0: " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}
	UDPNetworkUtility::GetSingleton().Open(0);

	std::cout << "Initialized SDL_net 2.0" << std::endl;

	//setting up SDL2_ttf
	if (TTF_Init()) {
		std::ostringstream msg;
		msg << "Failed to initialize SDL_ttf 2.0: " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}

	std::cout << "Initialized SDL_ttf 2.0" << std::endl;

	//-------------------------
	//debug output
	//-------------------------

#define DEBUG_INTERNAL_VAR(x) std::cout << "\t" << #x << ": " << x << std::endl;

	std::cout << "Internal sizes:" << std::endl;

	DEBUG_INTERNAL_VAR(NETWORK_VERSION);
	DEBUG_INTERNAL_VAR(sizeof(Region::type_t));
	DEBUG_INTERNAL_VAR(sizeof(Region));
	DEBUG_INTERNAL_VAR(REGION_WIDTH);
	DEBUG_INTERNAL_VAR(REGION_HEIGHT);
	DEBUG_INTERNAL_VAR(REGION_DEPTH);
	DEBUG_INTERNAL_VAR(REGION_TILE_FOOTPRINT);
	DEBUG_INTERNAL_VAR(REGION_SOLID_FOOTPRINT);
	DEBUG_INTERNAL_VAR(PACKET_STRING_SIZE);
	DEBUG_INTERNAL_VAR(PACKET_BUFFER_SIZE);
	DEBUG_INTERNAL_VAR(MAX_PACKET_SIZE);
	DEBUG_INTERNAL_VAR(static_cast<int>(SerialPacketType::LAST));

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
	//load the first scene
	ProcessSceneSignal(SceneSignal::FIRST);

	//fixed frame rate
	typedef std::chrono::steady_clock Clock;

	Clock::time_point simTime = Clock::now();
	Clock::time_point realTime;
	constexpr std::chrono::duration<int, std::milli> frameDelay(16); //~60FPS

	//the game loop continues until the scenes signal QUIT
	while(activeScene->GetSceneSignal() != SceneSignal::QUIT) {
		//switch scenes if necessary
		if(activeScene->GetSceneSignal() != SceneSignal::CONTINUE) {
			ProcessSceneSignal(activeScene->GetSceneSignal());
			continue;
		}

		//update the current time
		realTime = Clock::now();

		//simulate the game or give the machine a break
		if (simTime < realTime) {
			while(simTime < realTime) {
				//call the user defined functions
				activeScene->FrameStart();
				ProcessEvents();
				activeScene->Update();
				activeScene->FrameEnd();

				//step to the next frame
				simTime += frameDelay;
			}
		}
		else {
			SDL_Delay(1);
		}

		SDL_RenderClear(renderer);
		activeScene->RenderFrame(renderer);
		SDL_RenderPresent(renderer);
	}

	//cleanup
	ClearScene();
}

void ClientApplication::Quit() {
	//clean up after the program
	std::cout << "Shutting down" << std::endl;
	UDPNetworkUtility::GetSingleton().Close();
	TTF_Quit();
	SDLNet_Quit();
	BaseScene::SetRenderer(nullptr);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	std::cout << "Clean exit" << std::endl;
}

//-------------------------
//Scene management
//-------------------------

void ClientApplication::ProcessEvents() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				activeScene->QuitEvent();
			break;

			case SDL_MOUSEMOTION:
				activeScene->MouseMotion(event.motion);
			break;

			case SDL_MOUSEBUTTONDOWN:
				activeScene->MouseButtonDown(event.button);
			break;

			case SDL_MOUSEBUTTONUP:
				activeScene->MouseButtonUp(event.button);
			break;

			case SDL_MOUSEWHEEL:
				activeScene->MouseWheel(event.wheel);
			break;

			case SDL_KEYDOWN:
				activeScene->KeyDown(event.key);
			break;

			case SDL_KEYUP:
				activeScene->KeyUp(event.key);
			break;

			//TODO: (9) joystick and controller events

			//window events are handled internally
			case SDL_WINDOWEVENT:
				switch(event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						SDL_RenderSetLogicalSize(renderer, event.window.data1, event.window.data2);
					break;
				}
			break;
		}
	}
}

//Add the custom scene headers here
#include "splash_screen.hpp"
#include "main_menu.hpp"
#include "options_menu.hpp"
#include "lobby_menu.hpp"
#include "world.hpp"
#include "disconnected_screen.hpp"

void ClientApplication::ProcessSceneSignal(SceneSignal signal) {
	//BUG: #16 Resources are being reloaded between scenes
	ClearScene();
	switch(signal) {
		//add scene creation calls here
		case SceneSignal::FIRST:
		case SceneSignal::SPLASHSCREEN:
			activeScene = new SplashScreen(window);
		break;
		case SceneSignal::MAINMENU:
			activeScene = new MainMenu();
		break;
		case SceneSignal::OPTIONSMENU:
			activeScene = new OptionsMenu();
		break;
		case SceneSignal::LOBBYMENU:
			activeScene = new LobbyMenu(&clientIndex, &accountIndex);
		break;
		case SceneSignal::WORLD:
			activeScene = new World(&clientIndex, &accountIndex);
		break;
		case SceneSignal::DISCONNECTEDSCREEN:
			activeScene = new DisconnectedScreen();
		break;
		default: {
			std::ostringstream msg;
			msg << "Failed to recognize the scene signal: " << signal;
			throw(std::logic_error(msg.str()));
		}
	}
}

void ClientApplication::ClearScene() {
	delete activeScene;
	activeScene = nullptr;
}
