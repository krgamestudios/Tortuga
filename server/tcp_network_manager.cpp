#include "tcp_network_manager.hpp"

TCPNetworkManager::TCPNetworkManager() {
	maxConnections = currentConnections = 0;
}

TCPNetworkManager::~TCPNetworkManager() {
	//
}

void TCPNetworkManager::Init(int maxConnections) {
	//
}

void TCPNetworkManager::Quit() {
	//
}

int TCPNetworkManager::AcceptConnection() {
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

int TCPNetworkManager::GetMaxConnections() {
	//
}

int TCPNetworkManager::GetCurrentConnections() {
	//
}
