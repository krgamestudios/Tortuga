#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include "scene_list.h"
#include "base_scene.h"

#include "SDL/SDL.h"

class SceneManager {
public:
	/* Public access members */
	SceneManager();
	~SceneManager();

	void Init();
	void Proc();
	void Quit();

private:
	/* Private access members */
	void LoadScene(SceneList sceneIndex);
	void UnloadScene();

	BaseScene* activeScene;
};

#endif
