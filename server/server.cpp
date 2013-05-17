#include "server.hpp"

#include <stdexcept>
#include <iostream>
#include <algorithm>

using namespace std;

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
	for (auto it : socketList) {
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
		socketList.push_back(sock);
	}
	else {
		delete sock;
	}
	//accept updates from the clients
	string input;
	for_each(socketList.begin(), socketList.end(), [&input](TCPSocket* sock){ //why use for_each & lamdas?? to give logan a brain hemorrhage
		char buffer[512];
		memset(buffer, 0, 512);
		sock->Recv(buffer, 512);
		input += buffer;
	});
	//read the updates from the clients into internal containers
	if (input.size()) {
		cout << "dumping input from the network" << endl;
		cout << input << endl;
		input.clear();
	}
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
	for (auto it : socketList) {
		it->Send(s.c_str(), s.length());
	}
}
