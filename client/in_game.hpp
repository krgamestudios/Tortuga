#ifndef INGAME_HPP_
#define INGAME_HPP_

#include "base_scene.hpp"

#include "delta.hpp"
#include "player.hpp"
#include "surface_manager.hpp"

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

	//members
	Delta delta;
	SurfaceManager surfaceMgr;
	Player* player;
};

#endif
