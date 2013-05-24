#include "udp_network_utility.hpp"

#include <stdexcept>

void UDPNetworkUtility::Open(int port, int packSize) {
	if (!(socket = SDLNet_UDP_Open(port))) {
		Close();
		throw(std::runtime_error("Failed to open a UDP socket"));
	}

	if (!(packOut = SDLNet_AllocPacket(packSize))) {
		Close();
		throw(std::runtime_error("Failed to allocate the out packet"));
	}

	if (!(packIn = SDLNet_AllocPacket(packSize))) {
		Close();
		throw(std::runtime_error("Failed to allocate the in packet"));
	}
}

void UDPNetworkUtility::Close() {
	SDLNet_UDP_Close(socket);
	SDLNet_FreePacket(packOut);
	SDLNet_FreePacket(packIn);
	socket = nullptr;
	packOut = nullptr;
	packIn = nullptr;
}

int UDPNetworkUtility::Bind(const char* ip, int port, int channel) {
	IPaddress add;
	if (SDLNet_ResolveHost(&add, ip, port) == -1) {
		throw(std::runtime_error("Failed to resolve a host"));
	}

	return Bind(&add, channel);
}

int UDPNetworkUtility::Bind(IPaddress* add, int channel) {
	int ret = SDLNet_UDP_Bind(socket, channel, add);

	if (ret == -1) {
		throw(std::runtime_error("Failed to bind to a channel"));
	}

	return ret;
}

void UDPNetworkUtility::Unbind(int channel) {
	SDLNet_UDP_Unbind(socket, channel);
}

int UDPNetworkUtility::Send(const char* ip, int port, void* data, int len) {
	IPaddress add;
	if (SDLNet_ResolveHost(&add, ip, port) == -1) {
		throw(std::runtime_error("Failed to resolve a host"));
	}

	Send(&add, data, len);
}

int UDPNetworkUtility::Send(IPaddress* add, void* data, int len) {
	if (len > packOut->maxlen) {
		throw(std::runtime_error("Failed to copy the data into the packet"));
	}
	memset(packOut->data, 0, packOut->maxlen);
	memcpy(packOut->data, data, len);
	packOut->len = len;
	packOut->address = *add;

	int ret = SDLNet_UDP_Send(socket, -1, packOut);

	if (ret <= 0) {
		throw(std::runtime_error("Failed to send a packet"));
	}

	return ret;
}

int UDPNetworkUtility::Send(int channel, void* data, int len) {
	if (len > packOut->maxlen) {
		throw(std::runtime_error("Failed to copy the data into the packet"));
	}
	memset(packOut->data, 0, packOut->maxlen);
	memcpy(packOut->data, data, len);
	packOut->len = len;

	int ret = SDLNet_UDP_Send(socket, channel, packOut);

	if (ret <= 0) {
		throw(std::runtime_error("Failed to send a packet"));
	}

	return ret;
}

int UDPNetworkUtility::Receive() {
	memset(packIn->data, 0, packIn->maxlen);
	int ret = SDLNet_UDP_Recv(socket, packIn);

	if (ret < 0) {
		throw(std::runtime_error("Unknown network error occured"));
	}

	return ret;
}
