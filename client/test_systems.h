#ifndef TESTSYSTEMS_H_
#define TESTSYSTEMS_H_

#include "base_scene.h"

class TestSystems : public BaseScene {
public:
	/* Public access members */
	TestSystems();
	virtual ~TestSystems();

protected:
	/* Frame loop */
	virtual void FrameStart();
	virtual void FrameEnd();
	virtual void Update();
	virtual void Render(SDL_Surface* const);

	/* Event handlers */
	virtual void MouseMotion		(SDL_MouseMotionEvent const&);
	virtual void MouseButtonDown	(SDL_MouseButtonEvent const&);
	virtual void MouseButtonUp		(SDL_MouseButtonEvent const&);
	virtual void KeyDown			(SDL_KeyboardEvent const&);
	virtual void KeyUp				(SDL_KeyboardEvent const&);
};

#endif
