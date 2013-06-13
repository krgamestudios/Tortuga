#ifndef OPTIONSCREEN_HPP_
#define OPTIONSCREEN_HPP_

#include "base_scene.hpp"
#include "service_locator.hpp"

#include "surface_manager.hpp"
#include "button.hpp"

class OptionScreen : public BaseScene {
public:
	/* Public access members */
	OptionScreen();
	~OptionScreen();

protected:
	/* Frame loop */
	void Render(SDL_Surface* const);

	/* Event handlers */
	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	void MouseButtonUp(SDL_MouseButtonEvent const&);
	void KeyDown(SDL_KeyboardEvent const&);

	SurfaceManager* surfaceMgr = ServiceLocator<SurfaceManager>::Get();
	Button backButton;
};

#endif
