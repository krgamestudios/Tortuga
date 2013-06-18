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
#ifndef SPLASHSCREEN_HPP_
#define SPLASHSCREEN_HPP_

#include "base_scene.hpp"
#include "defines.hpp"
#include "service_locator.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "image.hpp"

#include <chrono>

class SplashScreen : public BaseScene {
public:
	/* Public access members */
	SplashScreen();
	~SplashScreen();

protected:
	/* Frame loop */
	void RunFrame(double delta);
	void RenderFrame();
	void LoadResources();

	bool loaded = false;
	ConfigUtility* configUtil = ServiceLocator<ConfigUtility>::Get();
	SurfaceManager* surfaceMgr = ServiceLocator<SurfaceManager>::Get();
	Image logo;
	Clock::time_point start = Clock::now();
};

#endif
