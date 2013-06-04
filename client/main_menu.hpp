#ifndef MAINMENU_HPP_
#define MAINMENU_HPP_

#include "base_scene.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"

#include "button.hpp"

#include <string>
#include <map>

class MainMenu : public BaseScene {
public:
	//Public access members
	MainMenu(ConfigUtility*, SurfaceManager*);
	~MainMenu();

protected:
	//Frame loop
	void FrameStart();
	void Update(double delta);
	void FrameEnd();
	void Render(SDL_Surface* const);

	//Event handlers
	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	void MouseButtonUp(SDL_MouseButtonEvent const&);
	void KeyDown(SDL_KeyboardEvent const&);
	void KeyUp(SDL_KeyboardEvent const&);

	//globals
	ConfigUtility* configUtil;
	SurfaceManager* surfaceMgr;

	//members
	std::map<std::string, Button*> buttonMap;
};

#endif
