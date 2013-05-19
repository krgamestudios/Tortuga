#include "udp_network_utility.hpp"

#include "SDL_net/SDL_net.h"

#include <iostream>
#include <string>

using namespace std;

int main(int, char**) {
	if (SDLNet_Init()) {
		cerr << "Failed to init SDL_net" << endl;
		return -1;
	}
	UDPNetworkUtility netUtil;

	netUtil.Open(0, 512);

	string s = "Hello world";
	netUtil.Send("127.0.0.1", 2000, (void*)s.c_str(), s.length());

	netUtil.Close();

	SDLNet_Quit();
	return 0;
}