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

	pager.SetOnNew([](Region* const ptr){
		printf("New Region: %d, %d\n", ptr->GetX(), ptr->GetY());
	});

	pager.SetOnDelete([](Region* const ptr){
		printf("Delete Region: %d, %d\n", ptr->GetX(), ptr->GetY());
	});

	sheetList.push_front(TileSheet());
	TileSheet* sheetOne = &sheetList.front();
	sheetList.push_front(TileSheet());
	TileSheet* sheetTwo = &sheetList.front();

	sheetOne->LoadSurface("rsc\\terrain.bmp", 32, 32);
	sheetTwo->LoadSurface("rsc\\terrain.bmp", 32, 32);

	cout << "--Sheet debug data--" << endl;
	cout << "TileSheet::rangeEnd: " << TileSheet::GetRangeEnd() << endl;
	cout << "sheetOne.totalCount: " << sheetOne->GetTotalCount() << endl;
	cout << "sheetOne.begin: " << sheetOne->GetBegin() << endl;
	cout << "sheetOne.end: " << sheetOne->GetEnd() << endl;
	cout << "sheetTwo.totalCount: " << sheetTwo->GetTotalCount() << endl;
	cout << "sheetTwo.begin: " << sheetTwo->GetBegin() << endl;
	cout << "sheetTwo.end: " << sheetTwo->GetEnd() << endl;
	cout << "--end debug data--" << endl;
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
	for (auto& regionIter : *pager.GetRegions()) {
		for (auto& tileIter : *regionIter.GetTiles()) {
			for (auto& sheetIter : sheetList) {
				if (sheetIter.InRange(tileIter.tileIndex)) {
					sheetIter.DrawTo(screen, tileIter.x + regionIter.GetX(), tileIter.y + regionIter.GetY(), tileIter.tileIndex);
				}
			}
		}
	}
}

//-------------------------
//Event handlers
//-------------------------

void EditorScene::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//
}

void EditorScene::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	switch(button.button) {
		case SDL_BUTTON_LEFT: {
				Region* ptr = pager.GetRegion(
					snapToBase(pager.GetWidth(), button.x),
					snapToBase(pager.GetHeight(), button.y)
				);

				cout << "New Tile: " <<
				ptr->NewTileA({
					snapToBase(32, button.x),
					snapToBase(32, button.y),
					0,
					32,
					32,
					incrementer
				})
				<< endl;
				;

				if (++incrementer >= 180) {
					incrementer = 0;
				}
			}

	}
}

void EditorScene::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	//
}

void EditorScene::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
			break;
	}
}

void EditorScene::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
