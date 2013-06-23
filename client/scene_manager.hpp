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
#ifndef SCENEMANAGER_HPP_
#define SCENEMANAGER_HPP_

#include "scene_list.hpp"
#include "base_scene.hpp"
#include "singleton.hpp"
#include "packet_type.hpp"
#include "information_manager.hpp"

#include "config_utility.hpp"
#include "surface_manager.hpp"
#include "udp_network_utility.hpp"

#include "SDL/SDL.h"

class SceneManager {
public:
	/* Public access members */
	SceneManager();
	~SceneManager();

	void Init();
	void Proc();
	void Quit();

	SceneManager(SceneManager const&) = delete;
	SceneManager(SceneManager const&&) = delete;
private:
	/* Private access members */
	void LoadScene(SceneList sceneIndex);
	void UnloadScene();

	BaseScene* activeScene = nullptr;

	ConfigUtility* configUtil = Singleton<ConfigUtility>::Get();
	SurfaceManager* surfaceMgr = Singleton<SurfaceManager>::Get();
	UDPNetworkUtility* netUtil = Singleton<UDPNetworkUtility>::Get();
	InformationManager* infoMgr = Singleton<InformationManager>::Get();
};

#endif
