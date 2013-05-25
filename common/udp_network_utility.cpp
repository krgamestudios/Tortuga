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
