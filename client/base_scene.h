#ifndef BASESCENE_H_
#define BASESCENE_H_

#include "scene_list.h"

#include "SDL/SDL.h"

class BaseScene {
public:
	/* Public access members */
	BaseScene();
	virtual ~BaseScene();

	/* Program control */
	static SDL_Surface* SetScreen(int w, int h, int bpp = 0, Uint32 flags = SDL_HWSURFACE|SDL_DOUBLEBUF);
	static SDL_Surface* GetScreen();

	SceneList SetNextScene(SceneList sceneIndex);
	SceneList GetNextScene() const;

	/* Frame loop */
	virtual void RunFrame();

protected:
	virtual void FrameStart() {}
	virtual void FrameEnd() {}
	virtual void Update() {}
	virtual void Render(SDL_Surface* const screen) {}

	/* Event handlers */
	virtual void HandleEvents();

	virtual void QuitEvent			() { SetNextScene(SceneList::QUIT); }
	virtual void MouseMotion		(SDL_MouseMotionEvent const&) {}
	virtual void MouseButtonDown	(SDL_MouseButtonEvent const&) {}
	virtual void MouseButtonUp		(SDL_MouseButtonEvent const&) {}
	virtual void KeyDown			(SDL_KeyboardEvent const&) {}
	virtual void KeyUp				(SDL_KeyboardEvent const&) {}

#ifdef USE_EVENT_JOYSTICK
	//TODO: joystick/gamepad support
#endif

#ifdef USE_EVENT_UNKNOWN
	virtual void UnknownEvent		(SDL_Event const&) {}
#endif

private:
	static SDL_Surface* screen;
	SceneList nextScene;
};

#endif
