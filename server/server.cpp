#include "server.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

Server::Server() {
	running = false;
}

Server::~Server() {
	//
}

void Server::Init() {
	NetworkInit();

	config.Load("config.cfg");
	servSock.Open(config.Int("port"));

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
	for (auto it : sockVec) {
		it->Close();
		delete it;
	}
	servSock.Close();
	NetworkQuit();
}

void Server::HandleInput() {
	//accept new connections
	TCPSocket* sock = new TCPSocket;
	if (servSock.Accept(sock)) {
		sockVec.push_back(sock);
	}
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
	//...
	//selective updates to existing connectons
	string s = "hello world";
	for (auto it : sockVec) {
		it->Send(s.c_str(), s.length());
	}
}
