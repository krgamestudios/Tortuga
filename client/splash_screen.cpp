#include "splash_screen.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

SplashScreen::SplashScreen() {
#ifdef DEBUG
	cout << "entering SplashScreen" << endl;
#endif
	logo.SetSurface(surfaceMgr->Load("splash-logo", configUtil->String("logos") + "/krstudios.bmp"));
}

SplashScreen::~SplashScreen() {
	surfaceMgr->Free("splash-logo");
#ifdef DEBUG
	cout << "leaving SplashScreen" << endl;
#endif
}

//-------------------------
//Frame loop
//-------------------------

void SplashScreen::RunFrame(double delta) {
	HandleEvents();
	if (!loaded) {
		//never repeat this
		loaded = true;

		//quick draw
		RenderFrame();

		LoadResources();
	}

	if (Clock::now() - start > std::chrono::duration<int>(1)) {
		SetNextScene(SceneList::MAINMENU);
	}
}

void SplashScreen::RenderFrame() {
	SDL_FillRect(GetScreen(), 0, 0);
	int x = (GetScreen()->w - logo.GetClipW()) / 2;
	int y = (GetScreen()->h - logo.GetClipH()) / 2;
	logo.DrawTo(GetScreen(), x, y);
	SDL_Flip(GetScreen());
}

void SplashScreen::LoadResources() {
	//standard
	surfaceMgr->Load("font", configUtil->String("fonts") + "/pokemon_dark_font.bmp");
	surfaceMgr->Load("button", configUtil->String("interface") + "/button.bmp");

	//debugging
	surfaceMgr->Load("elliot", configUtil->String("sprites") + "/elliot2.bmp");
	surfaceMgr->Load("coa", configUtil->String("sprites") + "/coa2.bmp");
	surfaceMgr->Load("flower", configUtil->String("sprites") + "/aniflower.bmp");
	surfaceMgr->Load("terrain", configUtil->String("tilesets") + "/terrain.bmp");
	//TODO
}
