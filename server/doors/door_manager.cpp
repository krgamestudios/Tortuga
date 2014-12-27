/* Copyright: (c) Kayne Ruse 2013-2015
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
#include "door_manager.hpp"

int DoorManager::Create(std::string, Vector2) {
	//TODO
}

int DoorManager::Load(std::string, Vector2) {
	//TODO
}

int DoorManager::Save(int uid) {
	//TODO
}

void DoorManager::Unload(int uid) {
	//TODO
}

void DoorManager::Delete(int uid) {
	//TODO
}

void DoorManager::UnloadAll() {
	//TODO
}

void DoorManager::UnloadIf(std::function<bool(std::pair<const int, DoorData>)> fn) {
	//TODO
}

DoorData* DoorManager::Get(int uid) {
	//TODO
}

int DoorManager::GetLoadedCount() {
	//TODO
}

int DoorManager::GetTotalCount() {
	//TODO
}

std::map<int, DoorData>* DoorManager::GetContainer() {
	//TODO
}
