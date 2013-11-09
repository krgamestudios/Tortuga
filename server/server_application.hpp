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
#ifndef SERVERAPPLICATION_HPP_
#define SERVERAPPLICATION_HPP_

#include "udp_network_utility.hpp"
#include "network_queue.hpp"

#include "sqlite3/sqlite3.h"
#include "SDL/SDL.h"
#include "SDL/SDL_thread.h"

#include <list>

//The main application class
class ServerApplication {
public:
	//standard functions
	ServerApplication();
	~ServerApplication();

	void Init(int argc, char** argv);
	void Loop();
	void Quit();

	friend int networkQueueThread(void*);
private:
	void HandlePacket(NetworkPacket);

	//members
	bool running = false;

	//networking
	UDPNetworkUtility networkUtil;
	NetworkQueue networkQueue;
	SDL_Thread* networkQueueThread = nullptr;

	//database
	sqlite3* database = nullptr;

	//clients
	struct ClientEntry {
		static int indexCounter;
		int index = indexCounter++;
		IPaddress add = {0, 0};
	};
	std::list<ClientEntry> clientEntries;
};

#endif
