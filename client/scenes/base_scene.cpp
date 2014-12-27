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
#include "base_scene.hpp"

#include <stdexcept>

//-------------------------
//Static declarations
//-------------------------

SDL_Surface* BaseScene::screen = nullptr;

//-------------------------
//Public access members
//-------------------------

BaseScene::BaseScene() {
	//
}

BaseScene::~BaseScene() {
	//
}

//-------------------------
//Program control
//-------------------------

SDL_Surface* BaseScene::SetScreen(int w, int h, int bpp, Uint32 flags) {
	if (!bpp) {
		bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
	}

	screen = SDL_SetVideoMode(w, h, bpp, flags);

	if (!screen) {
		throw(std::runtime_error("Failed to create the screen surface"));
	}

	return screen;
}

SDL_Surface* BaseScene::GetScreen() {
	return screen;
}

SceneList BaseScene::SetNextScene(SceneList sceneIndex) {
	return nextScene = sceneIndex;
}

SceneList BaseScene::GetNextScene() const {
	return nextScene;
}

//-------------------------
//Frame loop
//-------------------------

void BaseScene::RunFrame() {
	FrameStart();
	HandleEvents();
	Update();
	FrameEnd();
}

void BaseScene::RenderFrame() {
	SDL_FillRect(screen, 0, 0);
	Render(screen);
	SDL_Flip(screen);
	SDL_Delay(10);
}

//-------------------------
//Event handlers
//-------------------------

void BaseScene::HandleEvents() {
	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				QuitEvent();
			break;

			case SDL_VIDEORESIZE:
				SetScreen(event.resize.w, event.resize.h, 0, screen->flags);
			break;

			case SDL_MOUSEMOTION:
				MouseMotion(event.motion);
			break;

			case SDL_MOUSEBUTTONDOWN:
				MouseButtonDown(event.button);
			break;

			case SDL_MOUSEBUTTONUP:
				MouseButtonUp(event.button);
			break;

			case SDL_KEYDOWN:
				KeyDown(event.key);
			break;

			case SDL_KEYUP:
				KeyUp(event.key);
			break;

#ifdef USE_EVENT_JOYSTICK
			//TODO: joystick/gamepad support
#endif

#ifdef USE_EVENT_UNKNOWN
			default:
				UnknownEvent(event);
			break;
#endif
		}//switch
	}//while
}
