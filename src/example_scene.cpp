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

#include <iostream>

ExampleScene::ExampleScene(lua_State* L) {
	lua = L;

	tileSheet.Load(GetRenderer(), "./rsc/terrain.bmp", 32, 32);

	//set the pager's hook
	regionPager.SetLuaState(lua);

	//load the file as a chunk
	luaL_loadfile(lua, "./rsc/startup.lua");

	//push the pager as an arg
	lua_pushlightuserdata(lua, static_cast<void*>(&regionPager));

	//run the function
	lua_pcall(lua, 1, LUA_MULTRET, 0);
}

ExampleScene::~ExampleScene() {
	//
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
		tileSheet.DrawRegionTo(renderer, &(*it), camera.x, camera.y, camera.scale, camera.scale);
	}
}

//-------------------------
//input events
//-------------------------

void ExampleScene::MouseMotion(SDL_MouseMotionEvent const& event) {
	//right mouse button moves the camera
	if (event.state & SDL_BUTTON_RMASK) {
		camera.x -= event.xrel;
		camera.y -= event.yrel;
	}
}

void ExampleScene::MouseButtonDown(SDL_MouseButtonEvent const& event) {
	switch(event.button) {
		case SDL_BUTTON_LEFT:
			//change the selected tile
			regionPager.SetTile((event.x + camera.x) / 32, (event.y + camera.y) / 32, layer, selection);
		break;
	}
}

void ExampleScene::MouseButtonUp(SDL_MouseButtonEvent const& event) {
	//
}

void ExampleScene::MouseWheel(SDL_MouseWheelEvent const& event) {
	//
}

void ExampleScene::KeyDown(SDL_KeyboardEvent const& event) {
	//preference as a default
	switch(event.keysym.sym) {
		case SDLK_ESCAPE:
			QuitEvent();
		break;
	}
}

void ExampleScene::KeyUp(SDL_KeyboardEvent const& event) {
	//
}
