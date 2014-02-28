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

#include <cstdio>
#include <iostream>
#include <stdexcept>

using namespace std;

//-------------------------
//Public access members
//-------------------------

EditorScene::EditorScene(ConfigUtility* const arg1):
	config(*arg1),
	pager(20, 20, 3)
{
	//create the debugging "window"
	debugInfo.CreateSurface(256, 256);

	//setup the utility objects
	font.LoadSurface(config["dir.fonts"] + "pk_white_8.bmp");

	buttonImage.LoadSurface(config["dir.interface"] + "button_menu.bmp");
	buttonImage.SetClipH(buttonImage.GetClipH()/3);

	//setup the menu bar
	menuBar.SetFont(&font);
	menuBar.SetImage(&buttonImage);

	menuBar.SetEntries({
		{"File", "New", "Open", "Save", "Save As...", "Close", "Exit"},
		{"Edit", "Set Tile", "Load Sheet", "Delete Sheet", "Metadata", "Run Script"},
		{"Debugging", "Debug On", "Debug Off", "Toggle Debug", "Testificate"}
	});

	//debug
	tsheet.Load("rsc\\graphics\\tilesets\\sand.bmp", 12, 3);
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
	//debug
	for (int i = 0; i < pager.GetRegionWidth()*2; i++) {
		for (int j = 0; j < pager.GetRegionHeight()*2; j++) {
			for (int k = 0; k < pager.GetRegionDepth(); k++) {
				tsheet.DrawTo(
					screen,
					i*tsheet.GetTileW()-camera.x,
					j*tsheet.GetTileH()-camera.y,
					pager.GetTile(i,j,k)
				);
			}
		}
	}

	//draw a big bar across the top
	buttonImage.SetClipY(0);
	for (int i = 0; i < screen->w; i += buttonImage.GetClipW()) {
		buttonImage.DrawTo(screen, i, 0);
	}

	//draw the menu bar
	menuBar.DrawTo(screen);

	//draw some debugging info
	if (debugOpen) {
		SDL_FillRect(debugInfo.GetSurface(), 0, 0);
		DrawToDebugInfo(string("camera.x: ") + to_string_custom(camera.x), 0);
		DrawToDebugInfo(string("camera.y: ") + to_string_custom(camera.y), 1);
		debugInfo.DrawTo(screen, screen->w - debugInfo.GetClipW(), buttonImage.GetClipH());
	}
}

void EditorScene::DrawToDebugInfo(std::string str, int line) {
	//draw the debug info on the right
	font.DrawStringTo(
		str,
		debugInfo.GetSurface(),
		debugInfo.GetClipW() - str.size() * font.GetCharW(),
		font.GetCharH() * line
	);
}

//-------------------------
//Event handlers
//-------------------------

void EditorScene::MouseMotion(SDL_MouseMotionEvent const& motion) {
	menuBar.MouseMotion(motion);

	if (motion.state & SDL_BUTTON_RMASK) {
		camera.x -= motion.xrel;
		camera.y -= motion.yrel;
	}
}

void EditorScene::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	menuBar.MouseButtonDown(button);
}

void EditorScene::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	int entry, drop;
	menuBar.MouseButtonUp(button, &entry, &drop);

	//manage input from the menu bar
	switch(entry) {
		case 0: //File
			switch(drop) {
				case 0:
					//TODO: NEW
				break;

				case 1:
					//TODO: OPEN
				break;

				case 2:
					//TODO: SAVE
				break;

				case 3:
					//TODO: SAVE AS
				break;

				case 4:
					//TODO: CLOSE
				break;

				case 5: {
					//EXIT
					SDL_Event e;
					e.type = SDL_QUIT;
					SDL_PushEvent(&e);
				}
				break;
			}
		break;

		case 1: //Edit
			switch(drop) {
				case 0:
					//TODO: SET TILE
				break;

				case 1:
					//TODO: LOAD SHEET
				break;

				case 2:
					//TODO: DELETE SHEET
				break;

				case 3:
					//TODO: METADATA
				break;

				case 4:
					//TODO: RUN SCRIPT
				break;
			}
		break;

		case 2: //Debug
			switch(drop) {
				case 0:
					debugOpen = true;
				break;

				case 1:
					debugOpen = false;
				break;

				case 2:
					debugOpen = !debugOpen;
				break;

				case 3:
					SetNextScene(SceneList::TESTIFICATESCENE);
				break;
			}
		break;
	}
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

		case SDLK_TAB:
			debugOpen = !debugOpen;
		break;
	}
}

void EditorScene::KeyUp(SDL_KeyboardEvent const& key) {
	//
}
