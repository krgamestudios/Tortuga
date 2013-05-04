#include "server.hpp"

#include "SDL/SDL.h"
#include "SDL_net/SDL_net.h"

#include <stdexcept>
#include <iostream>

using namespace std;

Server::Server() {
	running = true;
	config.Load("config.cfg");
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
	while(running) {
		HandleInput();
		UpdateWorld();
		HandleOutput();

		//debug
		running = false;
	}
}

void Server::Quit() {
	SDLNet_Quit();
}

void Server::HandleInput() {
	//accept new connections
	//...
	//accept updates from the clients
	//...
	//read the updates from the clients into internal containers
	//...
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
