#include "splash.hpp"

#include <iostream>

using namespace std;

//-------------------------
//Public access members
//-------------------------

Splash::Splash(ConfigUtility* cUtil, SurfaceManager* sMgr) {
#ifdef DEBUG
	cout << "entering Splash" << endl;
#endif
	loaded = false;
	start = clock();
	configUtil = cUtil;
	surfaceMgr = sMgr;

	logo = new Image(surfaceMgr->Load("splash-logo", configUtil->String("logos") + "/krstudios.bmp"));
}

Splash::~Splash() {
	delete logo;
	surfaceMgr->Free("splash-logo");
#ifdef DEBUG
	cout << "leaving Splash" << endl;
#endif
}

void Splash::RunFrame() {
	//skip any event handling here
	SDL_Event event;
	while(SDL_PollEvent(&event));

	//draw the logo in the middle of the screen
	int x = (GetScreen()->w - logo->GetClipW()) / 2;
	int y = (GetScreen()->h - logo->GetClipH()) / 2;

	logo->DrawTo(GetScreen(),x,y);
	SDL_Flip(GetScreen());

	if (!loaded) {
		LoadResources();
		loaded = true;
	}

	if (clock() - start > CLOCKS_PER_SEC*3) {
		SetNextScene(SceneList::MAINMENU);
	}
}

void Splash::LoadResources() {
	//load the global resources here
	surfaceMgr->Load("font", configUtil->String("fonts") + "/pokemon_dark_font.bmp");
	surfaceMgr->Load("button", configUtil->String("interface") + "/button.bmp");
	surfaceMgr->Load("elliot", configUtil->String("sprites") + "/elliot2.bmp");
	surfaceMgr->Load("coa", configUtil->String("sprites") + "/coa2.bmp");
	surfaceMgr->Load("flower", configUtil->String("sprites") + "/aniflower.bmp");
	surfaceMgr->Load("terrain", configUtil->String("tilesets") + "/terrain.bmp");
	//TODO
}