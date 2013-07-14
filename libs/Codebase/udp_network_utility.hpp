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
#ifndef UDPNETWORKUTILITY_HPP_
#define UDPNETWORKUTILITY_HPP_

#include "SDL/SDL_net.h"

class UDPNetworkUtility {
public:
	UDPNetworkUtility() = default;
	~UDPNetworkUtility() = default;

	void Open(int port, int packSize);
	void Close();

	//bind to an available channel
	int Bind(const char* ip, int port) {
		Bind(ip, port, -1);
	}
	int Bind(IPaddress* add) {
		Bind(add, -1);
	}

	//bind to certain channel
	int Bind(const char* ip, int port, int channel);
	int Bind(IPaddress* add, int channel);
	void Unbind(int channel);

	IPaddress* GetIPAddress(int channel) {
		return SDLNet_UDP_GetPeerAddress(socket, channel);
	}

	int Send(const char* ip, int port, void* data, int len);
	int Send(IPaddress* add, void* data, int len);
	int Send(int channel, void* data, int len);
	int SendAll(void* data, int len);
	int Receive();

	void* GetOutData() const {
		return reinterpret_cast<void*>(packOut->data);
	};
	void* GetInData() const {
		return reinterpret_cast<void*>(packIn->data);
	};
	UDPpacket* GetOutPacket() const {
		return packOut;
	}
	UDPpacket* GetInPacket() const {
		return packIn;
	}
	UDPsocket GetSocket() const {
		return socket;
	}
private:
	UDPsocket socket = nullptr;
	UDPpacket* packOut = nullptr;
	UDPpacket* packIn = nullptr;
};

#endif
