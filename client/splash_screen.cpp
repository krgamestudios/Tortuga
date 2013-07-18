/* Copyright: (c) Kayne Ruse 2013
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
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

	if (std::chrono::steady_clock::now() - start > std::chrono::duration<int>(1)) {
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
	surfaceMgr->Load("font", configUtil->String("fonts") + "/pk_white_8.bmp");
	surfaceMgr->Load("button", configUtil->String("interface") + "/button_menu.bmp");

	//debugging
	surfaceMgr->Load("elliot", configUtil->String("sprites") + "/elliot2.bmp");
	surfaceMgr->Load("coa", configUtil->String("sprites") + "/coa2.bmp");
	surfaceMgr->Load("flower", configUtil->String("sprites") + "/aniflower.bmp");
	surfaceMgr->Load("terrain", configUtil->String("tilesets") + "/terrain.bmp");
	//TODO
}
