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
#ifndef MENUBAR_HPP_
#define MENUBAR_HPP_

#include "image.hpp"
#include "raster_font.hpp"
#include "button.hpp"

#include <string>
#include <vector>

/* I've redesigned this so that the contents of the menu bar can't change during run time.
 * This is more restrictive but I'm focusing on getting this working first.
 * The Image and Font pointers must be set before the text data is entered.
 * 
 * This class needs a rewrite.
*/

class MenuBar {
public:
	MenuBar() = default;
	~MenuBar() = default;

	//yet another draw function
	void DrawTo(SDL_Surface* const dest);

	//user inputs
	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	void MouseButtonUp(SDL_MouseButtonEvent const&, int* entry, int* button);

	//manage the entries & buttons
	void SetEntries(std::vector<std::vector<std::string>> info);
	void ClearEntries() { entries.clear(); }

	//Accessors and mutators
	Image* SetImage(Image* const ptr) { return image = ptr; }
	Image* GetImage() { return image; }
	RasterFont* SetFont(RasterFont* const ptr) { return font = ptr; }
	RasterFont* GetFont() { return font; }

private:
	class MenuBarEntry;

	std::vector<MenuBarEntry> entries;

	Image* image = nullptr;
	RasterFont* font = nullptr;
};

class MenuBar::MenuBarEntry {
public:
	MenuBarEntry() = default;
	~MenuBarEntry() = default;

	void DrawTo(SDL_Surface* const dest);

	void MouseMotion(SDL_MouseMotionEvent const&);
	void MouseButtonDown(SDL_MouseButtonEvent const&);
	int MouseButtonUp(SDL_MouseButtonEvent const&);

private:
	Button mainButton;

	std::vector<Button> dropButtons;
	bool open = false;

	friend class MenuBar;
};

#endif
