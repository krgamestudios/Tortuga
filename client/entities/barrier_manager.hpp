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

#include "base_barrier.hpp"

#include <list>
#include <string>

class BarrierManager {
public:
	BarrierManager() = default;
	~BarrierManager() = default;

	void DrawTo(SDL_Renderer* const, Sint16 x, Sint16 y, double scaleX = 1.0, double scaleY = 1.0);

	//NOTE: don't use these while you have barriers loaded
	void LoadBaseImage(SDL_Renderer* renderer, std::string fname);
	void UnloadBaseImage();
	void LoadTemplateImages(SDL_Renderer* renderer, std::string spriteDir, std::list<std::string> names);
	void UnloadTemplateImages();

	BaseBarrier* Create(int index);
	void Unload(int i);
	void UnloadAll();

	int Size();

	BaseBarrier* Find(int i);
	std::map<int, BaseBarrier>* GetContainer();
	std::map<std::string, Image>* GetTemplateContainer();

private:
	Image baseImage;
	std::map<std::string, Image> templateImages;
	std::map<int, BaseBarrier> barrierMap;
};