#ifndef SPLASHSCREEN_HPP_
#define SPLASHSCREEN_HPP_

#include "base_scene.hpp"
#include "service_locator.hpp"
#include "defines.hpp"

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
