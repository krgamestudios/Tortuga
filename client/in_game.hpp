#ifndef INGAME_HPP_
#define INGAME_HPP_

#include "base_scene.hpp"

#include "player_manager.hpp"

#include "delta.hpp"
#include "frame_rate.hpp"
#include "config_utility.hpp"
#include "surface_manager.hpp"

#include <string>

class InGame : public BaseScene {
public:
	//Public access members
	InGame(ConfigUtility*, SurfaceManager*);
	virtual ~InGame();

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
	void NewPlayer(int index, std::string avatarName, int x, int y);
	void SwitchToPlayer(int index);

	//members
	PlayerManager playerMgr;

	Delta delta;
	FrameRate frameRate;
	ConfigUtility* configUtil;
	SurfaceManager* surfaceMgr;

	int playerCounter;
	int currentPlayer;
};

#endif
