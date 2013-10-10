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
#include "editor_scene.hpp"

#include "utility.hpp"

#include <stdexcept>
#include <iostream>
#include <cstdio>

using namespace std;

//-------------------------
//Public access members
//-------------------------

EditorScene::EditorScene() {
	//32 * 32 sized tiles
	pager.SetWidth(32*4);
	pager.SetHeight(32*4);

	font.LoadSurface("rsc\\graphics\\fonts\\pk_white_8.bmp");
	buttonImage.LoadSurface("rsc\\graphics\\interface\\button_menu.bmp");
	buttonImage.SetClipH(buttonImage.GetClipH()/3);

	menuBar.SetFont(&font);
	menuBar.SetImage(&buttonImage);

	menuBar.SetEntries({
		{"File", "New", "Open", "Save", "Save As...", "Close", "Exit"},
		{"Edit", "Null"}
	});

//	pager.SetOnNew([](Region* const ptr){
//		printf("New Region: %d, %d\n", ptr->GetX(), ptr->GetY());
//	});

//	pager.SetOnDelete([](Region* const ptr){
//		printf("Delete Region: %d, %d\n", ptr->GetX(), ptr->GetY());
//	});

	sheetList.push_front(TileSheet());
	sheetList.front().LoadSurface("rsc\\graphics\\tilesets\\terrain.bmp", 32, 32);
}

EditorScene::~EditorScene() {
	//
}

//-------------------------
//Frame loop
//-------------------------

void EditorScene::FrameStart() {
	//
}

void EditorScene::Update(double delta) {
	//
}

void EditorScene::FrameEnd() {
	//
}

void EditorScene::Render(SDL_Surface* const screen) {
	//draw the map
	pager.DrawTo(screen, &sheetList, camera.x, camera.y);

	//draw a big bar across the top
	for (int i = 0; i < screen->w; i += buttonImage.GetClipW()) {
		buttonImage.DrawTo(screen, i, 0);
	}

	//draw the menu bar
	menuBar.DrawTo(screen);
}

//-------------------------
//Event handlers
//-------------------------

void EditorScene::MouseMotion(SDL_MouseMotionEvent const& motion) {
	menuBar.MouseMotion(motion);

	if (motion.state & SDL_BUTTON_RMASK) {
		camera.x += motion.xrel;
		camera.y += motion.yrel;
	}
}

void EditorScene::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	menuBar.MouseButtonDown(button);

	if (button.button == SDL_BUTTON_LEFT && button.y >= buttonImage.GetClipH()) {
		Region* ptr = pager.GetRegion(
			snapToBase(pager.GetWidth(), button.x - camera.x),
			snapToBase(pager.GetHeight(), button.y - camera.y)
		);

		ptr->NewTileA({
			snapToBase(32, button.x - camera.x),
			snapToBase(32, button.y - camera.y),
			0,
			32,
			32,
			0
		});
	}
}

void EditorScene::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	int entry, drop;
	menuBar.MouseButtonUp(button, &entry, &drop);

	cout << "Menu: (" << entry << "," << drop << ")" << endl;
}

void EditorScene::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;

		case SDLK_SPACE:
			camera.x = 0;
			camera.y = 0;
		break;
	}
}

void EditorScene::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
