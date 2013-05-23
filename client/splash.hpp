#ifndef SPLASH_HPP_
#define SPLASH_HPP_

#include "base_scene.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "image.hpp"

#include <ctime>

class Splash : public BaseScene {
public:
	Splash(ConfigUtility*, SurfaceManager*);
	virtual ~Splash();

protected:
	virtual void RunFrame();

	void LoadResources();

	//globals
	ConfigUtility* configUtil;
	SurfaceManager* surfaceMgr;

	//members
	bool loaded = false;
	time_t start = clock();
	Image* logo = nullptr;
};

#endif
