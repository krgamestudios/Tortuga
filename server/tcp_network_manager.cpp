#include "tcp_network_manager.hpp"

#include <stdexcept>

TCPNetworkManager::TCPNetworkManager() {
	//
}

TCPNetworkManager::~TCPNetworkManager() {
	//
}

void TCPNetworkManager::Init(Uint16 port, int maxSockets) {
	IPaddress add;
	if (SDLNet_ResolveHost(&add, nullptr, port)) {
		throw(std::runtime_error("Failed to resolve the host"));
	}
	if (!(sock = SDLNet_TCP_Open(&add))) {
		throw(std::runtime_error("Failed to create the server socket"));
	}
	if (!(clientSocks = SDLNet_AllocSocketSet(maxSockets))) {
		throw(std::runtime_error("Failed to allocate the socket set"));
	}
}

void TCPNetworkManager::Quit() {
	SDLNet_FreeSocketSet(clientSocks);
	SDLNet_TCP_Close(sock);
}

int TCPNetworkManager::AcceptConnections() {
	//
}

int TCPNetworkManager::CheckSockets() {
	//
}

int TCPNetworkManager::Send(int index, const void* data, int len) {
	//
}

int TCPNetworkManager::SendAll(const void* data, int len) {
	//
}

int TCPNetworkManager::OpenSocket(const char* host, int port) {
	//
}

TCPsocket TCPNetworkManager::GetSocket(int index) {
	//
}

int TCPNetworkManager::CloseSocket(int index) {
	//
}

int TCPNetworkManager::GetMaxConnections() const {
	//
}

int TCPNetworkManager::GetCurrentConnections() const {
	//
}
