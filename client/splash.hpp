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

	bool loaded;
	time_t start;

	ConfigUtility* configUtil;
	SurfaceManager* surfaceMgr;
	Image* logo;
};

#endif
