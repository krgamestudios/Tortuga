/* Copyright: (c) Kayne Ruse 2015
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
#include "example_scene.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

ExampleScene::ExampleScene(lua_State* L) {
	lua = L;

	//TODO: non-hardcoded source
	tileSheet.Load(GetRenderer(), "./rsc/terrain.bmp", 32, 32);

	//set the pager's hook
	regionPager.SetLuaState(lua);

	//load the file as a chunk
	luaL_loadfile(lua, "./rsc/startup.lua");

	//push the pager as an arg
	lua_pushlightuserdata(lua, static_cast<void*>(&regionPager));

	//run the function
	lua_pcall(lua, 1, LUA_MULTRET, 0);

	//DEBUG: cam jump
	camera.x = -3000;
	camera.y = -1500;
	camera.scale = 0.25;

	//DEBUG: testing buttons
	buttonBG.Load(GetRenderer(), "./rsc/button.png");
	font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 12);
	if (!font) {
		std::ostringstream msg;
		msg << "Failed to load a font: " << SDL_GetError();
		throw(std::runtime_error(msg.str()));
	}
	button.SetBackgroundTexture(GetRenderer(), buttonBG.GetTexture());
	button.SetText(GetRenderer(), font, "BUTTON TEXT", {0, 0, 0, 255});
	button.SetX(400);
	button.SetY(200);
//	{140, 62, 54, 255}
}

ExampleScene::~ExampleScene() {
	TTF_CloseFont(font);
}

//-------------------------
//frame phases
//-------------------------

void ExampleScene::FrameStart() {
	//
}

void ExampleScene::Update() {
	//
}

void ExampleScene::FrameEnd() {
	//
}

void ExampleScene::RenderFrame(SDL_Renderer* renderer) {
	for (std::list<Region>::iterator it = regionPager.GetContainer()->begin(); it != regionPager.GetContainer()->end(); it++) {
		//NOTE: Graphical scaling is done internally
		tileSheet.DrawRegionTo(renderer, &(*it), camera.x, camera.y, camera.scale, camera.scale);
	}

	//DEBUG: testing UI
	button.DrawTo(renderer);

	SDL_Texture* tex = renderPlainText(renderer, font, "STRING\nTEXT", {255, 255, 255, 255});
	int w = 0, h = 0;
	SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
	SDL_Rect d = {0, 0, w, h};
	SDL_RenderCopy(renderer, tex, nullptr, &d);
	SDL_DestroyTexture(tex);
}

//-------------------------
//input events
//-------------------------

void ExampleScene::MouseMotion(SDL_MouseMotionEvent const& event) {
	Button::State state = button.MouseMotion(event);

	if (state == Button::State::PRESSED) {
		//motion while pressed
		return;
	}

	//right mouse button moves the camera
	if (event.state & SDL_BUTTON_RMASK) {
		camera.x -= event.xrel / camera.scale;
		camera.y -= event.yrel / camera.scale;
	}
}

void ExampleScene::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	Button::State state = button.MouseButtonDown(event);

	//catch button input
	if (state == Button::State::PRESSED) {
		//TODO: do stuff
		std::cout << "pressed" << std::endl;
		return;
	}

	switch(event.button) {
		case SDL_BUTTON_LEFT: {
			//DOCS: broke this down into several lines for clarity
			//these are the "real" click positions, relative to the map
			//(scaled input is transformed into unscaled tile selection)
			int fieldX = event.x / camera.scale + camera.x;
			int fieldY = event.y / camera.scale + camera.y;

			//these are the x & y indexes of the selected tile
			//NOTE: the terniary operator is used to circumvent an error with integer devision
			int tileX = (fieldX >= 0 ? fieldX : fieldX - tileSheet.GetTileW()) / tileSheet.GetTileW();
			int tileY = (fieldY >= 0 ? fieldY : fieldY - tileSheet.GetTileH()) / tileSheet.GetTileH();

			//finally, call the method
			regionPager.SetTile(tileX, tileY, layer, selection);
		}
		break;
	}
}

void ExampleScene::MouseButtonUp(SDL_MouseButtonEvent const& event) {
	Button::State state = button.MouseButtonUp(event);

	//catch button input
	if (state == Button::State::RELEASED) {
		std::cout << "released" << std::endl;
		return;
	}
}

void ExampleScene::MouseWheel(SDL_MouseWheelEvent const& event) {
	constexpr double scaleMod = 1.2;
	constexpr double scaleUpperBound = 3.0;
	constexpr double scaleLowerBound = 0.5;

	//zoom out
	if (event.y < 0) { //downscroll
		if (camera.scale / scaleMod <= scaleLowerBound) {
			camera.scale = scaleLowerBound;
		}
		else {
			camera.scale /= scaleMod;
		}
	}

	//zoom in
	if (event.y > 0) { //upscroll
		if (camera.scale * scaleMod >= scaleUpperBound) {
			camera.scale = scaleUpperBound;
		}
		else {
			camera.scale *= scaleMod;
		}
	}
}

void ExampleScene::KeyDown(SDL_KeyboardEvent const& event) {
	//preference as a default
	switch(event.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;

		case SDLK_r:
			SetSceneSignal(SceneSignal::EXAMPLE_SCENE);
		break;

		case SDLK_SPACE:
			camera.scale = 1.0;
		break;
	}
}

void ExampleScene::KeyUp(SDL_KeyboardEvent const& event) {
	//
}
