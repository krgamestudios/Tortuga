/* Copyright: (c) Kayne Ruse 2013, 2014
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

#include "serial.hpp"

#include <stdexcept>

//DOCS: memset() is used before sending a packet to remove old data; you don't want to send sensitive data over the network
//NOTE: don't confuse SerialPacket with UDPpacket

void UDPNetworkUtility::Open(int port) {
	socket = SDLNet_UDP_Open(port);
	packet = SDLNet_AllocPacket(PACKET_BUFFER_SIZE);
	if (!socket || !packet) {
		Close();
		throw(std::runtime_error("Failed to open UDPNetworkUtility"));
	}
}

void UDPNetworkUtility::Close() {
	SDLNet_UDP_Close(socket);
	SDLNet_FreePacket(packet);
	socket = nullptr;
	packet = nullptr;
}

//-------------------------
//bind to a channel
//-------------------------

int UDPNetworkUtility::Bind(const char* ip, int port, int channel) {
	IPaddress add;
	if (SDLNet_ResolveHost(&add, ip, port) == -1) {
		throw(std::runtime_error("Failed to resolve a host"));
	}

	return Bind(&add, channel);
}

int UDPNetworkUtility::Bind(IPaddress* add, int channel) {
	int ret = SDLNet_UDP_Bind(socket, channel, add);

	if (ret < 0) {
		throw(std::runtime_error("Failed to bind to a channel"));
	}

	return ret;
}

void UDPNetworkUtility::Unbind(int channel) {
	SDLNet_UDP_Unbind(socket, channel);
}

//-------------------------
//send a buffer
//-------------------------

int UDPNetworkUtility::SendTo(const char* ip, int port, void* data, int len) {
	IPaddress add;
	if (SDLNet_ResolveHost(&add, ip, port) == -1) {
		throw(std::runtime_error("Failed to resolve a host"));
	}

	SendTo(&add, data, len);
}

int UDPNetworkUtility::SendTo(IPaddress* add, void* data, int len) {
	if (len > packet->maxlen) {
		throw(std::runtime_error("The buffer is to large for the UDPpacket"));
	}
	memset(packet->data, 0, packet->maxlen);
	memcpy(packet->data, data, len);
	packet->len = len;
	packet->address = *add;

	int ret = SDLNet_UDP_Send(socket, -1, packet);

	if (ret <= 0) {
		throw(std::runtime_error("Failed to send a packet"));
	}

	return ret;
}

int UDPNetworkUtility::SendTo(int channel, void* data, int len) {
	if (len > packet->maxlen) {
		throw(std::runtime_error("The buffer is to large for the UDPpacket"));
	}
	memset(packet->data, 0, packet->maxlen);
	memcpy(packet->data, data, len);
	packet->len = len;

	int ret = SDLNet_UDP_Send(socket, channel, packet);

	if (ret <= 0) {
		throw(std::runtime_error("Failed to send a packet"));
	}

	return ret;
}

int UDPNetworkUtility::SendToAllChannels(void* data, int len) {
	if (len > packet->maxlen) {
		throw(std::runtime_error("The buffer is to large for the UDPpacket"));
	}
	memset(packet->data, 0, packet->maxlen);
	memcpy(packet->data, data, len);
	packet->len = len;

	int sent = 0;

	//send to all bound channels
	for (int i = 0; i < SDLNET_MAX_UDPCHANNELS; i++) {
		if (SDLNet_UDP_GetPeerAddress(socket, i)) {
			sent += SDLNet_UDP_Send(socket, i, packet);
		}
	}

	return sent;
}

//TODO: put a void* and int* parameter list here
int UDPNetworkUtility::Receive() {
	memset(packet->data, 0, packet->maxlen);
	int ret = SDLNet_UDP_Recv(socket, packet);

	if (ret < 0) {
		throw(std::runtime_error("Unknown network error occured"));
	}

	return ret;
}

//-------------------------
//send a SerialPacket
//-------------------------

int UDPNetworkUtility::SendTo(const char* ip, int port, SerialPacket* serialPacket) {
	IPaddress add;
	if (SDLNet_ResolveHost(&add, ip, port) == -1) {
		throw(std::runtime_error("Failed to resolve a host"));
	}

	SendTo(&add, serialPacket);
}

int UDPNetworkUtility::SendTo(IPaddress* add, SerialPacket* serialPacket) {
	memset(packet->data, 0, packet->maxlen);
	serializePacket(serialPacket, packet->data);
	packet->len = PACKET_BUFFER_SIZE;
	packet->address = *add;

	int ret = SDLNet_UDP_Send(socket, -1, packet);

	if (ret <= 0) {
		throw(std::runtime_error("Failed to send a packet"));
	}

	return ret;
}

int UDPNetworkUtility::SendTo(int channel, SerialPacket* serialPacket) {
	memset(packet->data, 0, packet->maxlen);
	serializePacket(serialPacket, packet->data);
	packet->len = PACKET_BUFFER_SIZE;

	int ret = SDLNet_UDP_Send(socket, channel, packet);

	if (ret <= 0) {
		throw(std::runtime_error("Failed to send a packet"));
	}

	return ret;
}

int UDPNetworkUtility::SendToAllChannels(SerialPacket* serialPacket) {
	memset(packet->data, 0, packet->maxlen);
	serializePacket(serialPacket, packet->data);
	packet->len = PACKET_BUFFER_SIZE;

	int sent = 0;

	//send to all bound channels
	for (int i = 0; i < SDLNET_MAX_UDPCHANNELS; i++) {
		if (SDLNet_UDP_GetPeerAddress(socket, i)) {
			sent += SDLNet_UDP_Send(socket, i, packet);
		}
	}

	return sent;
}

int UDPNetworkUtility::Receive(SerialPacket* serialPacket) {
	memset(packet->data, 0, packet->maxlen);
	int ret = SDLNet_UDP_Recv(socket, packet);
	deserializePacket(serialPacket, packet->data);
	serialPacket->srcAddress = packet->address;

	if (ret < 0) {
		throw(std::runtime_error("Unknown network error occured"));
	}

	return ret;
}