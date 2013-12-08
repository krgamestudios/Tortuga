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
#include "world_room.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

int worldRoomThread(void* arg) {
	WorldRoom* room = reinterpret_cast<WorldRoom*>(arg);
	try {
		room->Init();
		room->Loop();
		room->Quit();
	}
	catch(exception& e) {
		cerr << "Fatal room error: " << e.what() << endl;
		return 1;
	}
	return 0;
}

WorldRoom::WorldRoom(PlayerMap const& arg1):
	playerMap(arg1)
{
	//
}

WorldRoom::~WorldRoom() {
	//
}

void WorldRoom::OpenRoom() {
	if (running) {
		throw(std::runtime_error("Cannot open a room that is already running"));
	}

	running = true;
	thread = SDL_CreateThread(worldRoomThread, this);
}

void WorldRoom::CloseRoom() {
	running = false;
	SDL_WaitThread(thread, nullptr);
}

void WorldRoom::KillRoom() {
	running = false;
	SDL_KillThread(thread);
}

void WorldRoom::Init() {
	//
}

void WorldRoom::Loop() {
	while(running) {
		while(networkInQueue.Size() > 0) {
			HandlePacket(networkInQueue.PopFront());
		}
		SDL_Delay(10);
	}
}

void WorldRoom::Quit() {
	//
}

void WorldRoom::HandlePacket(NetworkPacket packet) {
	cout << "packet received" << endl;
}
