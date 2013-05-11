#ifndef MAINMENU_HPP_
#define MAINMENU_HPP_

#include "base_scene.hpp"

class MainMenu : public BaseScene {
public:
	/* Public access members */
	MainMenu();
	virtual ~MainMenu();

protected:
	/* Frame loop */
	virtual void FrameStart();
	virtual void FrameEnd();
	virtual void Update();
	virtual void Render(SDL_Surface* const);

	/* Event handlers */
	virtual void MouseMotion(SDL_MouseMotionEvent const&);
	virtual void MouseButtonDown(SDL_MouseButtonEvent const&);
	virtual void MouseButtonUp(SDL_MouseButtonEvent const&);
	virtual void KeyDown(SDL_KeyboardEvent const&);
	virtual void KeyUp(SDL_KeyboardEvent const&);
};

#endif
