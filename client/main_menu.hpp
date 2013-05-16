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
	virtual ~MainMenu();

protected:
	//Frame loop
	virtual void FrameStart();
	virtual void FrameEnd();
	virtual void Update();
	virtual void Render(SDL_Surface* const);

	//Event handlers
	virtual void MouseMotion(SDL_MouseMotionEvent const&);
	virtual void MouseButtonDown(SDL_MouseButtonEvent const&);
	virtual void MouseButtonUp(SDL_MouseButtonEvent const&);
	virtual void KeyDown(SDL_KeyboardEvent const&);
	virtual void KeyUp(SDL_KeyboardEvent const&);

	//utilities
	typedef std::map<std::string, Button*> ButtonMap;

	//singletons
	ConfigUtility* configUtil;
	SurfaceManager* surfaceMgr;

	//members
	ButtonMap buttonMap;
};

#endif
