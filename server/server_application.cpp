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
#include "server_application.hpp"

#include "network_packet.hpp"

#include <stdexcept>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

//-------------------------
//Declarations
//-------------------------

int ServerApplication::ClientEntry::indexCounter = 0;

//-------------------------
//Define the network thread
//-------------------------

int networkQueueThread(void* ptr) {
	ServerApplication* app = reinterpret_cast<ServerApplication*>(ptr);
	NetworkPacket packet;

	while(app->running) {
		//suck in the waiting packets
		while(app->networkUtil.Receive()) {
			memcpy(&packet, app->networkUtil.GetInData(), sizeof(NetworkPacket));
			//this is important: keep track of the source address
			packet.meta.srcAddress = app->networkUtil.GetInPacket()->address;
			app->networkQueue.Push(packet);
		}
		SDL_Delay(10);
	}
	return 0;
}

//-------------------------
//Define the ServerApplication
//-------------------------

ServerApplication::ServerApplication() {
	//
}

ServerApplication::~ServerApplication() {
	//
}

void ServerApplication::Init(int argc, char** argv) {
	//TODO: proper command line option parsing

	//Check prerequisites
	if (!sqlite3_threadsafe()) {
		throw(runtime_error("Cannot run without thread safety"));
	}
	cout << "Thread safety confirmed" << endl;

	if (running) {
		throw(std::runtime_error("Multiple calls to ServerApplication::Init() is not allowed"));
	}
	running = true;

	//Init SDL
	if (SDL_Init(0)) {
		throw(runtime_error("Failed to initialize SDL"));
	}
	cout << "initialized SDL" << endl;

	//Init SDL_net
	if (SDLNet_Init()) {
		throw(runtime_error("Failed to init SDL_net"));
	}
	networkUtil.Open(21795, sizeof(NetworkPacket));
	cout << "initialized SDL_net" << endl;

	//Init SQL
	string dbname = (argc > 1) ? argv[1] : argv[0]; //fancy and unnecessary
	int ret = sqlite3_open_v2((dbname + ".db").c_str(), &database, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|SQLITE_OPEN_FULLMUTEX, nullptr);
	if (ret != SQLITE_OK || !database) {
		throw(runtime_error("Failed to open the server database"));
	}
	cout << "initialized SQL" << endl;
	cout << "Database filename: \"" << dbname << ".db\"" << endl;

	//TODO: move this into a function?
	//Run setup scripts
	ifstream is("rsc\\scripts\\setup_server.sql");
	if (!is.is_open()) {
		throw(runtime_error("Failed to run database setup script"));
	}
	string script;
	getline(is, script, '\0');
	is.close();
	sqlite3_exec(database, script.c_str(), nullptr, nullptr, nullptr);

	//setup the threads
	networkQueueThread = SDL_CreateThread(&::networkQueueThread, this);
	if (!networkQueueThread) {
		throw(runtime_error("Failed to create the networkQueueThread"));
	}
	cout << "initialized networkQueueThread" << endl;

	//debugging
	NetworkPacket packet;
	packet.meta.type = NetworkPacket::Type::PING;
	strcpy(packet.serverInfo.name, "foo");
	networkUtil.Send("127.0.0.1", 21795, &packet, sizeof(NetworkPacket));
}

void ServerApplication::Loop() {
	//debugging
	SDL_Delay(1000);

	while(networkQueue.Size() > 0) {
		try {
			HandlePacket(networkQueue.Pop());
		}
		catch(exception& e) {
			cerr << "Network Error: " << e.what() << endl;
		}
	};
}

void ServerApplication::Quit() {
	//catch all signal
	running = false;

	//members
	SDL_WaitThread(networkQueueThread, nullptr);
	networkUtil.Close();

	//APIs
	sqlite3_close_v2(database);
	SDLNet_Quit();
	SDL_Quit();
}

void ServerApplication::HandlePacket(NetworkPacket packet) {
	switch(packet.meta.type) {
		case NetworkPacket::Type::PING:
			//NOT USED
			//debugging
			cout << packet.serverInfo.name << endl;
		break;
		case NetworkPacket::Type::PONG:
			//NOT USED
		break;
		case NetworkPacket::Type::BROADCAST_REQUEST:
			//
		break;
		case NetworkPacket::Type::BROADCAST_RESPONSE:
			//
		break;
		case NetworkPacket::Type::JOIN_REQUEST:
			//
		break;
		case NetworkPacket::Type::JOIN_RESPONSE:
			//
		break;
		case NetworkPacket::Type::DISCONNECT:
			//
		break;
		case NetworkPacket::Type::SYNCHRONIZE:
			//
		break;

		//handle errors
		case NetworkPacket::Type::NONE:
			throw(runtime_error("NetworkPacket::Type::NONE encountered"));
		break;
		default:
			throw(runtime_error("Unknown NetworkPacket::Type encountered"));
		break;
	}
}