/* Copyright: (c) Kayne Ruse 2013-2016
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
#pragma once

#include "image.hpp"

#include <list>
#include <map>
#include <string>

template<typename T = class Image>
class CompositeImage {
public:
	CompositeImage() = default;
	~CompositeImage() = default;

	void Load(std::string spriteDir, std::list<std::string> nameList);
	void SetImageTextures(std::map<std::string, Image>& templateImages);
	void Free();

	void DrawTo(SDL_Renderer* const, Sint16 x, Sint16 y, double scaleX = 1.0, double scaleY = 1.0);

	//accessors & mutators
	T* Find(std::string name);

	bool Enable(std::string name);
	bool Disable(std::string name);
	void EnableAll();
	void DisableAll();

private:
	std::map<std::string, std::pair<bool, T> > imageMap;
};


template<typename T>
void CompositeImage<T>::Load(std::string spriteDir, std::list<std::string> nameList) {
	for (auto& it : nameList) {
		imageMap[it].first = true;
		imageMap[it].second.Load(spriteDir + it);
	}
}

template<typename T>
void CompositeImage<T>::SetImageTextures(std::map<std::string, Image>& templateImages) {
	for (auto& it : templateImages) {
		imageMap[it.first].first = true;
		imageMap[it.first].second.SetTexture(it.second.GetTexture());
	}
}

template<typename T>
void CompositeImage<T>::Free() {
	imageMap.clear();
}

template<typename T>
void CompositeImage<T>::DrawTo(SDL_Renderer* const dest, Sint16 x, Sint16 y, double scaleX, double scaleY) {
	//draw all members, regardless of internal ordering
	for (auto& it : imageMap) {
		if (it.first) {
			it.second.DrawTo(dest, x, y, scaleX, scaleY);
		}
	}
}


//-------------------------
//accessors & mutators
//-------------------------

template<typename T>
T* CompositeImage<T>::Find(std::string name) {
	auto it = imageMap.find(name);
	if (it == imageMap.end()) {
		return nullptr;
	}
	else {
		return &it.second;
	}
}

template<typename T>
bool CompositeImage<T>::Enable(std::string name) {
	auto it = imageMap.find(name);
	if (it == imageMap.end()) {
		return false;
	}
	else {
		imageMap[name].first = true;
		return true;
	}
}

template<typename T>
bool CompositeImage<T>::Disable(std::string name) {
	auto it = imageMap.find(name);
	if (it == imageMap.end()) {
		return false;
	}
	else {
		imageMap[name].first = false;
		return true;
	}
}

template<typename T>
void CompositeImage<T>::EnableAll() {
	for (auto& it : imageMap) {
		it.first = true;
	}
}

template<typename T>
void CompositeImage<T>::DisableAll() {
	for (auto& it : imageMap) {
		it.first = false;
	}
}
