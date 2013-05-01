#ifndef INGAME_HPP_
#define INGAME_HPP_

#include "base_scene.hpp"

#include "delta.hpp"
#include "frame_rate.hpp"
#include "player_manager.hpp"
#include "surface_manager.hpp"

#include <string>

class InGame : public BaseScene {
public:
	//Public access members
	InGame();
	virtual ~InGame();

protected:
	//Frame loop
	virtual void FrameStart();
	virtual void FrameEnd();
	virtual void Update();
	virtual void Render(SDL_Surface* const);

	//Event handlers
	virtual void MouseMotion		(SDL_MouseMotionEvent const&);
	virtual void MouseButtonDown	(SDL_MouseButtonEvent const&);
	virtual void MouseButtonUp		(SDL_MouseButtonEvent const&);
	virtual void KeyDown			(SDL_KeyboardEvent const&);
	virtual void KeyUp				(SDL_KeyboardEvent const&);

	//utilities
	void NewPlayer(int index, std::string avatarName, int x, int y);

	//members
	Delta delta;
	FrameRate frameRate;
	SurfaceManager surfaceMgr;
	PlayerManager playerMgr;

	Player* player;
};

#endif
