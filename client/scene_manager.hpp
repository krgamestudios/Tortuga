#ifndef SCENEMANAGER_HPP_
#define SCENEMANAGER_HPP_

#include "scene_list.hpp"
#include "base_scene.hpp"

#include "singleton.hpp"
#include "config_utility.hpp"

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

	BaseScene* activeScene;

	ConfigUtility* configUtil;
};

#endif
