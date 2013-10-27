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
#include "menu_bar.hpp"

#include <stdexcept>
#include <sstream>

void MenuBar::DrawTo(SDL_Surface* const dest) {
	for (auto& i : entries) {
		i.DrawTo(dest);
	}
}

void MenuBar::MouseMotion(SDL_MouseMotionEvent const& motion) {
	for (auto& i : entries) {
		i.MouseMotion(motion);
	}
}

void MenuBar::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	for (auto& i : entries) {
		i.MouseButtonDown(button);
	}
}

void MenuBar::MouseButtonUp(SDL_MouseButtonEvent const& button, int* entry, int* butt) {
	*entry = *butt = -1;
	int ret = -1;
	for (auto& i : entries) {
		ret = i.MouseButtonUp(button);

		if (ret != -1) {
			*entry = (&i - entries.data());
			*butt = ret;
		}
	}
}

void MenuBar::SetEntries(std::vector<std::vector<std::string>> info) {
	if (!image || !font) {
		throw(std::runtime_error("Surfaces not loaded into the menu bar"));
	}

	entries.clear();
	for (int i = 0; i < info.size(); i++) {
		//create the entry & the main button
		entries.push_back(MenuBarEntry());
		entries[i].mainButton.SetImage(image);
		entries[i].mainButton.SetFont(font);
		entries[i].mainButton.SetText(info[i][0]);
		entries[i].mainButton.SetX(i * image->GetClipW());
		entries[i].mainButton.SetY(0);
		for (int j = 0; j < info[i].size()-1; j++) {
			//create each drop button in this entry
			entries[i].dropButtons.push_back(Button());
			entries[i].dropButtons[j].SetImage(image);
			entries[i].dropButtons[j].SetFont(font);
			entries[i].dropButtons[j].SetText(info[i][j+1]);
			entries[i].dropButtons[j].SetX(i * image->GetClipW());
			entries[i].dropButtons[j].SetY((j+1) * image->GetClipH());
		}
	}
}

void MenuBar::MenuBarEntry::DrawTo(SDL_Surface* const dest) {
	//only draw the dropButtons in the user has this menu open
	mainButton.DrawTo(dest);

	if (!open) {
		return;
	}

	for (auto& i : dropButtons) {
		i.DrawTo(dest);
	}
}

void MenuBar::MenuBarEntry::MouseMotion(SDL_MouseMotionEvent const& motion) {
	//open the menu
	bool o = mainButton.MouseMotion(motion) == Button::State::PRESSED;

	if (!(open |= o)) {
		return;
	}

	for (auto& i : dropButtons) {
		//dragging down the menu
		o |= i.MouseMotion(motion) == Button::State::PRESSED;
	}

	open = o;
}

void MenuBar::MenuBarEntry::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	//open the menu
	if (!(open = mainButton.MouseButtonDown(button) == Button::State::PRESSED)) {
		return;
	}

	//update the others anyway
	for (auto& i : dropButtons) {
		i.MouseButtonDown(button);
	}
}

int MenuBar::MenuBarEntry::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	int ret = -1;
	mainButton.MouseButtonUp(button);

	for (auto& i : dropButtons) {
		//the user just released this button
		if (i.GetState() != i.MouseButtonUp(button) && i.GetState() == Button::State::HOVER && open) {
			//get this button's index
			ret = (&i - dropButtons.data());
		}
	}

	open = false;
	return ret;
}
