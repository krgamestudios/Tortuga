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
#include "barrier_manager.hpp"

void BarrierManager::DrawTo(SDL_Renderer* const dest, Sint16 x, Sint16 y, double scaleX, double scaleY) {
	for (auto& it : elementMap) {
		it.second.DrawTo(dest, x, y);
	}
}

void BarrierManager::LoadBaseImage(SDL_Renderer* renderer, std::string fname) {
	baseImage.Load(renderer, fname);
}

void BarrierManager::UnloadBaseImage() {
	baseImage.Free();
}

void BarrierManager::LoadTemplateImages(SDL_Renderer* renderer, std::string spriteDir, std::list<std::string> names) {
	//sprite names are file names only
	for (auto& it : names) {
		templateImages.emplace(it, Image(renderer, spriteDir + it));
	}
}

void BarrierManager::UnloadTemplateImages() {
	templateImages.clear();
}

BaseBarrier* BarrierManager::Create(int index) {
	elementMap.emplace(index, BaseBarrier(baseImage, templateImages));
	return &elementMap[index];
}

void BarrierManager::Unload(int i) {
	elementMap.erase(i);
}

void BarrierManager::UnloadAll() {
	elementMap.clear();
}

void BarrierManager::UnloadIf(std::function<bool(std::pair<const int, BaseBarrier const&>)> fn) {
	std::map<int, BaseBarrier>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}
}

int BarrierManager::Size() {
	return elementMap.size();
}

BaseBarrier* BarrierManager::Find(int i) {
	std::map<int, BaseBarrier>::iterator it = elementMap.find(i);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

std::map<int, BaseBarrier>* BarrierManager::GetContainer() {
	return &elementMap;
}

std::map<std::string, Image>* BarrierManager::GetTemplateContainer() {
	return &templateImages;
}