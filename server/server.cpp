#include "server.hpp"

#include "SDL/SDL.h"
#include "SDL_net/SDL_net.h"

#include <stdexcept>
#include <iostream>

using namespace std;

Server::Server() {
	running = true;
}

Server::~Server() {
	//
}

void Server::Init() {
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to init SDL_net"));
	}
	netMgr.Init(100);
}

void Server::Proc() {
	while(running) {
		HandleInput();
		UpdateWorld();
		HandleOutput();
	}
}

void Server::Quit() {
	netMgr.Quit();
	SDLNet_Quit();
}

void Server::HandleInput() {
	//accept new connections
	//accept updates from the clients
}

void Server::UpdateWorld() {
	//update internals
}

void Server::HandleOutput() {
	//send all information to new connections
	//selective updates to existing connectons
}
