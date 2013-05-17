#include "server.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

void Server::Init() {
	config.Load("config.cfg");
	running = true;
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
	//
}

void Server::HandleInput() {
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
