/* Copyright: (c) Kayne Ruse 2013, 2014
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
#ifndef BASESCENE_HPP_
#define BASESCENE_HPP_

#include "scene_list.hpp"

#include "SDL/SDL.h"

class BaseScene {
public:
	//Public access members
	BaseScene();
	virtual ~BaseScene();

	//Program control
	static SDL_Surface* SetScreen(int w, int h, int bpp = 0, Uint32 flags = SDL_HWSURFACE|SDL_DOUBLEBUF);
	static SDL_Surface* GetScreen();

	SceneList SetNextScene(SceneList sceneIndex);
	SceneList GetNextScene() const;

	//Frame loop
	virtual void RunFrame();
	virtual void RenderFrame();

protected:
	virtual void FrameStart() {}
	virtual void HandleEvents();
	virtual void Update() {}
	virtual void FrameEnd() {}
	virtual void Render(SDL_Surface* const screen) {}

	//Event handlers
	virtual void QuitEvent() { SetNextScene(SceneList::QUIT); }
	virtual void MouseMotion(SDL_MouseMotionEvent const&) {}
	virtual void MouseButtonDown(SDL_MouseButtonEvent const&) {}
	virtual void MouseButtonUp(SDL_MouseButtonEvent const&) {}
	virtual void KeyDown(SDL_KeyboardEvent const&) {}
	virtual void KeyUp(SDL_KeyboardEvent const&) {}

#ifdef USE_EVENT_JOYSTICK
	//TODO: joystick/gamepad support
#endif

#ifdef USE_EVENT_UNKNOWN
	virtual void UnknownEvent(SDL_Event const&) {}
#endif

private:
	static SDL_Surface* screen;
	SceneList nextScene = SceneList::CONTINUE;
};

#endif
