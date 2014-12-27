/* Copyright: (c) Kayne Ruse 2013-2015
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
#ifndef UDPNETWORKUTILITY_HPP_
#define UDPNETWORKUTILITY_HPP_

//common
#include "serial_packet_base.hpp"
#include "singleton.hpp"

//APIs
#include "SDL/SDL_net.h"

class UDPNetworkUtility : public Singleton<UDPNetworkUtility> {
public:
	void Open(int port);
	void Close();

	//bind to a channel
	int Bind(const char* ip, int port, int channel = -1);
	int Bind(IPaddress add, int channel = -1);
	void Unbind(int channel);

	IPaddress* GetIPAddress(int channel) {
		return SDLNet_UDP_GetPeerAddress(socket, channel);
	}

	//send a buffer
	int SendTo(const char* ip, int port, void* data, int len);
	int SendTo(IPaddress add, void* data, int len);
	int SendTo(int channel, void* data, int len);
	int SendToAllChannels(void* data, int len);
	int Receive();

	//send a SerialPacketBase
	int SendTo(const char* ip, int port, SerialPacketBase* serialPacket);
	int SendTo(IPaddress add, SerialPacketBase* serialPacket);
	int SendTo(int channel, SerialPacketBase* serialPacket);
	int SendToAllChannels(SerialPacketBase* serialPacket);
	int Receive(SerialPacketBase* serialPacket);

	//accessors
	UDPpacket* GetPacket() const {
		return packet;
	}
	UDPsocket GetSocket() const {
		return socket;
	}
private:
	friend Singleton<UDPNetworkUtility>;

	UDPNetworkUtility() = default;
	~UDPNetworkUtility() = default;

	UDPsocket socket = nullptr;
	UDPpacket* packet = nullptr;
};

#endif
