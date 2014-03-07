/* Copyright: (c) Kayne Ruse 2014
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
#ifndef CLIENTMANAGER_HPP_
#define CLIENTMANAGER_HPP_

#include "SDL/SDL_net.h"

#include <functional>
#include <map>

struct ClientEntry {
	IPaddress address;
};

class ClientManager {
public:
	//clarity typedefs
	typedef std::map<int, ClientEntry> Container;
	typedef Container::iterator Iterator;

	//returns the internal index
	int HandleConnection(IPaddress);
	int HandleDisconnection(int i);

	//lambdas
	void ForEach(std::function<void(Iterator)> fn);
	void EraseIf(std::function<bool(Iterator)> fn);

	//accessors
	ClientEntry* GetClient(int i);
	ClientEntry* GetClient(IPaddress);
	int Size() { return clientMap.size(); }
private:
	Container clientMap;
	int counter = 0;
};

#endif
