#include "server.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

void Server::Init() {
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to initialize SDL_net"));
	}
	config.Load("config.cfg");
	netUtil.Open(config.Integer("port"), 512);
	running = true;
}

void Server::Proc() {
	while(running) {
		HandleInput();
		UpdateWorld();
		HandleOutput();

		//debug
//		running = false;
	}
}

void Server::Quit() {
	netUtil.Close();
	SDLNet_Quit();
}

void Server::HandleInput() {
	while(netUtil.Receive()) {
		cout << reinterpret_cast<char*>(netUtil.GetInData()) << endl;
	}
	//accept new connections
	//accept updates from the clients
	//read the updates from the clients into internal containers
}

void Server::UpdateWorld() {
	//update internals ie.
	//	ai
	//	loot drops
}

void Server::HandleOutput() {
	//send all information to new connections
	//selective updates to existing connectons
}
