#include "server.hpp"

#include "SDL/SDL.h"
#include "SDL_net/SDL_net.h"

#include <stdexcept>

using namespace std;

Server::Server() {
	//
}

Server::~Server() {
	//
}

void Server::Init() {
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to init SDL_net"));
	}
}

void Server::Proc() {
	bool running = true;
	while(running) {
		//
	}
}

void Server::Quit() {
	SDLNet_Quit();
}
