/* Copyright: (c) Kayne Ruse 2013, 2014
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
#include "in_world.hpp"

#include "timer.hpp"

#include "utility.hpp"

#include "region_pager_api.hpp"
#include "tile_sheet_api.hpp"

#include <iostream>

//-------------------------
//Public access members
//-------------------------

InWorld::InWorld(lua_State* L): lua(L) {
	//register the pager
	lua_pushstring(lua, TORTUGA_REGION_PAGER_PSEUDO_INDEX);
	lua_pushlightuserdata(lua, &pager);
	lua_settable(L, LUA_REGISTRYINDEX);

	//register the tilesheet
	lua_pushstring(lua, TORTUGA_TILE_SHEET_PSEUDO_INDEX);
	lua_pushlightuserdata(lua, &tileSheet);
	lua_settable(L, LUA_REGISTRYINDEX);

	//setup the component objecrs
	pager.SetLuaState(lua);

	//get the config info
	lua_getglobal(lua, "config");
	lua_getfield(lua, -1, "dir");
	lua_getfield(lua, -1, "fonts");
	std::string fonts = lua_tostring(lua, -1);
	lua_getfield(lua, -2, "interface");
	std::string interface = lua_tostring(lua, -1);
	lua_getfield(lua, -3, "sprites");
	std::string sprites = lua_tostring(lua, -1);
	lua_getfield(lua, -4, "scripts");
	std::string scripts = lua_tostring(lua, -1);
	lua_pop(lua, 6);

	//run the additional scripts
	if (luaL_dofile(lua, (scripts + "in_world.lua").c_str())) {
		throw(std::runtime_error(std::string() + "Failed to run in_world.lua: " + lua_tostring(lua, -1) ));
	}

	//setup the utility objects
	buttonImage.LoadSurface(interface + "button_menu.bmp");
	buttonImage.SetClipH(buttonImage.GetClipH()/3);
	font.LoadSurface(fonts + "pk_white_8.bmp");

	//pass the utility objects
	backButton.SetImage(&buttonImage);
	backButton.SetFont(&font);

	//set the button positions
	backButton.SetX(50);
	backButton.SetY(50 + buttonImage.GetClipH() * 0);

	//set the button texts
	backButton.SetText("Back");

	//entities
	character.GetSprite()->LoadSurface(sprites + "elliot2.bmp", 4, 4);
	character.SetBoundingBox({0, 0,
		character.GetSprite()->GetImage()->GetClipW(),
		character.GetSprite()->GetImage()->GetClipH()
	});

	//setup the camera
	camera.margin.x = GetScreen()->w / 2 - character.GetSprite()->GetImage()->GetClipW() / 2;
	camera.margin.y = GetScreen()->h / 2 - character.GetSprite()->GetImage()->GetClipH() / 2;

	//debug
	//
}

InWorld::~InWorld() {
	//unregister the map components
	lua_pushstring(lua, TORTUGA_REGION_PAGER_PSEUDO_INDEX);
	lua_pushstring(lua, TORTUGA_TILE_SHEET_PSEUDO_INDEX);
	lua_pushnil(lua);
	lua_settable(lua, LUA_REGISTRYINDEX);
	lua_pushnil(lua);
	lua_settable(lua, LUA_REGISTRYINDEX);
}

//-------------------------
//Frame loop
//-------------------------

void InWorld::FrameStart() {
	//
}

void InWorld::Update(double delta) {
	//update the entities
	character.Update(delta);

	//check for collisions with the map
	BoundingBox wallBounds = {0, 0, tileSheet.GetTileW(), tileSheet.GetTileH()};
	const int xCount = character.GetBoundingBox().w / wallBounds.w + 1;
	const int yCount = character.GetBoundingBox().h / wallBounds.h + 1;

	for (int i = -1; i <= xCount; ++i) {
		for (int j = -1; j <= yCount; ++j) {
			//set the wall's position
			wallBounds.x = wallBounds.w * i + snapToBase((double)wallBounds.w, character.GetOrigin().x);
			wallBounds.y = wallBounds.h * j + snapToBase((double)wallBounds.h, character.GetOrigin().y);

			if (!pager.GetSolid(wallBounds.x / wallBounds.w, wallBounds.y / wallBounds.h)) {
				continue;
			}

			if ((character.GetOrigin() + character.GetBoundingBox()).CheckOverlap(wallBounds)) {
				character.SetOrigin(character.GetOrigin() - (character.GetMotion() * delta));
				character.SetMotion({0,0});
				character.CorrectSprite();
			}
		}
	}

	//update the camera
	camera.origin.x = character.GetOrigin().x - camera.margin.x;
	camera.origin.y = character.GetOrigin().y - camera.margin.y;

	//TODO: prune distant regions

	//summon nearby unloaded regions
	const int left = snapToBase(REGION_WIDTH, camera.origin.x / tileSheet.GetTileW());
	const int top = snapToBase(REGION_HEIGHT, camera.origin.y / tileSheet.GetTileH());
	const int right = snapToBase(REGION_WIDTH, (camera.origin.x + GetScreen()->w) / tileSheet.GetTileW());
	const int bottom = snapToBase(REGION_HEIGHT, (camera.origin.y + GetScreen()->h) / tileSheet.GetTileH());

	for (int i = left - REGION_WIDTH; i <= right + REGION_WIDTH; i += REGION_WIDTH) {
		for (int j = top - REGION_HEIGHT; j <= bottom + REGION_HEIGHT; j += REGION_HEIGHT) {
			pager.GetRegion(i, j);
		}
	}
}

void InWorld::FrameEnd() {
	//
}

void InWorld::RenderFrame() {
//	SDL_FillRect(GetScreen(), 0, 0);
	Render(GetScreen());
	SDL_Flip(GetScreen());
	fps.Calculate();
}

void InWorld::Render(SDL_Surface* const screen) {
	//draw all regions
	for (std::list<Region>::iterator it = pager.GetContainer()->begin(); it != pager.GetContainer()->end(); it++) {
		tileSheet.DrawRegionTo(screen, &(*it), camera.origin.x, camera.origin.y);
	}

	//draw the characters
	character.DrawTo(screen, camera.origin.x, camera.origin.y);

	//draw the UI
	backButton.DrawTo(screen);
	font.DrawStringTo(to_string_custom(fps.GetFrameRate()), screen, 0, 0);
}

//-------------------------
//Event handlers
//-------------------------

void InWorld::QuitEvent() {
	//exit the game
	SetNextScene(SceneList::QUIT);
}

void InWorld::MouseMotion(SDL_MouseMotionEvent const& motion) {
	backButton.MouseMotion(motion);
}

void InWorld::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	backButton.MouseButtonDown(button);
}

void InWorld::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (backButton.MouseButtonUp(button) == Button::State::HOVER) {
		SetNextScene(SceneList::MAINMENU);
	}
}

void InWorld::KeyDown(SDL_KeyboardEvent const& key) {
	//player movement
	Vector2 motion = character.GetMotion();
	switch(key.keysym.sym) {
		case SDLK_LEFT:
			motion.x -= CHARACTER_WALKING_SPEED;
		break;
		case SDLK_RIGHT:
			motion.x += CHARACTER_WALKING_SPEED;
		break;
		case SDLK_UP:
			motion.y -= CHARACTER_WALKING_SPEED;
		break;
		case SDLK_DOWN:
			motion.y += CHARACTER_WALKING_SPEED;
		break;
	}
	if (character.GetMotion() != motion) {
		character.SetMotion(motion);
		character.CorrectSprite();
	}
}

void InWorld::KeyUp(SDL_KeyboardEvent const& key) {
	//player movement
	Vector2 motion = character.GetMotion();
	switch(key.keysym.sym) {
		//NOTE: The use of min/max here are to prevent awkward movements
		case SDLK_LEFT:
			motion.x = std::min(motion.x + CHARACTER_WALKING_SPEED, 0.0);
		break;
		case SDLK_RIGHT:
			motion.x = std::max(motion.x - CHARACTER_WALKING_SPEED, 0.0);
		break;
		case SDLK_UP:
			motion.y = std::min(motion.y + CHARACTER_WALKING_SPEED, 0.0);
		break;
		case SDLK_DOWN:
			motion.y = std::max(motion.y - CHARACTER_WALKING_SPEED, 0.0);
		break;
	}
	if (character.GetMotion() != motion) {
		character.SetMotion(motion);
		character.CorrectSprite();
	}
}
