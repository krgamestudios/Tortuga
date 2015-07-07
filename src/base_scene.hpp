/* Copyright: (c) Kayne Ruse 2015
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
#pragma once

#include "scene_signal.hpp"

#include "SDL2/SDL.h"

class BaseScene {
public:
	BaseScene();
	virtual ~BaseScene();

	virtual void RunFrame();
	virtual void RenderFrame(SDL_Renderer*);

	static void SetRenderer(SDL_Renderer*);
	SceneSignal GetSceneSignal();

protected:
	//control
	static SDL_Renderer* GetRenderer();
	void SetSceneSignal(SceneSignal);

	//frame phases
	virtual void FrameStart();
	virtual void ProcessEvents();
	virtual void Update();
	virtual void FrameEnd();

	//input events
	virtual void QuitEvent();
	virtual void MouseMotion(SDL_MouseMotionEvent const& event);
	virtual void MouseButtonDown(SDL_MouseButtonEvent const& event);
	virtual void MouseButtonUp(SDL_MouseButtonEvent const& event);
	virtual void MouseWheel(SDL_MouseWheelEvent const& event);
	virtual void KeyDown(SDL_KeyboardEvent const& event);
	virtual void KeyUp(SDL_KeyboardEvent const& event);

	//TODO: joystick and controller events

private:
	static SDL_Renderer* rendererHandle;
	SceneSignal sceneSignal = SceneSignal::CONTINUE;
};