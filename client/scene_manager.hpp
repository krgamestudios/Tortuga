#ifndef SCENEMANAGER_HPP_
#define SCENEMANAGER_HPP_

#include "scene_list.hpp"
#include "base_scene.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "udp_network_utility.hpp"
#include "packet_list.hpp"

#include "SDL/SDL.h"

class SceneManager {
public:
	//Public access members
	SceneManager();
	~SceneManager();

	void Init();
	void Proc();
	void Quit();

private:
	//Private access members
	void LoadScene(SceneList sceneIndex);
	void UnloadScene();

	BaseScene* activeScene = nullptr;

	//globals
	ConfigUtility configUtil;
	SurfaceManager surfaceMgr;
	UDPNetworkUtility netUtil;
	int playerID = -1;
};

#endif
