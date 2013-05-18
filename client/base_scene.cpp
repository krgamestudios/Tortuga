#include "base_scene.hpp"

#include <stdexcept>

//-------------------------
//Static declarations
//-------------------------

SDL_Surface* BaseScene::screen = nullptr;

//-------------------------
//Public access members
//-------------------------

BaseScene::BaseScene() {
	nextScene = SceneList::CONTINUE;
}

BaseScene::~BaseScene() {
	//
}

//-------------------------
//Program control
//-------------------------

SDL_Surface* BaseScene::SetScreen(int w, int h, int bpp, Uint32 flags) {
	if (!bpp) {
		bpp = SDL_GetVideoInfo()->vfmt->BitsPerPixel;
	}

	screen = SDL_SetVideoMode(w, h, bpp, flags);

	if (!screen) {
		throw(std::runtime_error("Failed to create the screen surface"));
	}

	return screen;
}

SDL_Surface* BaseScene::GetScreen() {
	return screen;
}

SceneList BaseScene::SetNextScene(SceneList sceneIndex) {
	return nextScene = sceneIndex;
}

SceneList BaseScene::GetNextScene() const {
	return nextScene;
}

//-------------------------
//Frame loop
//-------------------------

void BaseScene::RunFrame() {
	FrameStart();
	HandleEvents();
	Update();
	SDL_FillRect(screen, 0, 0);
	Render(screen);
	SDL_Flip(screen);
	FrameEnd();
}

//-------------------------
//Event handlers
//-------------------------

void BaseScene::HandleEvents() {
	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				QuitEvent();
			break;

			case SDL_VIDEORESIZE:
				SetScreen(event.resize.w, event.resize.h, 0, screen->flags);
			break;

			case SDL_MOUSEMOTION:
				MouseMotion(event.motion);
			break;

			case SDL_MOUSEBUTTONDOWN:
				MouseButtonDown(event.button);
			break;

			case SDL_MOUSEBUTTONUP:
				MouseButtonUp(event.button);
			break;

			case SDL_KEYDOWN:
				KeyDown(event.key);
			break;

			case SDL_KEYUP:
				KeyUp(event.key);
			break;

#ifdef USE_EVENT_JOYSTICK
			//TODO: joystick/gamepad support
#endif

#ifdef USE_EVENT_UNKNOWN
			default:
				UnknownEvent(event);
			break;
#endif
		}//switch
	}//while
}
