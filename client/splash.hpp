#ifndef SPLASH_HPP_
#define SPLASH_HPP_

#include "base_scene.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "image.hpp"

#include <chrono>

class Splash : public BaseScene {
public:
	Splash(ConfigUtility*, SurfaceManager*);
	~Splash();

protected:
	typedef std::chrono::high_resolution_clock Clock;
	void RunFrame(double delta);
	void RenderFrame() {};

	void LoadResources();

	//globals
	ConfigUtility* configUtil;
	SurfaceManager* surfaceMgr;

	//members
	bool loaded = false;
	Clock::time_point start = Clock::now();
	Image* logo = nullptr;
};

#endif
