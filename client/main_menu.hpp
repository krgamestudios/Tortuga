#ifndef MAINMENU_HPP_
#define MAINMENU_HPP_

#include "base_scene.hpp"
#include "service_locator.hpp"

#include "surface_manager.hpp"
#include "button.hpp"

class MainMenu : public BaseScene {
public:
	/* Public access members */
	MainMenu();
	~MainMenu();

protected:
	/* Frame loop */
	void FrameStart();
	void Update(double delta);
	void FrameEnd();
	void Render(SDL_Surface* const);

	/* Event handlers */
	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	void MouseButtonUp(SDL_MouseButtonEvent const&);
	void KeyDown(SDL_KeyboardEvent const&);

	SurfaceManager* surfaceMgr = ServiceLocator<SurfaceManager>::Get();

	Button startButton;
	Button optionsButton;
	Button quitButton;
};

#endif
