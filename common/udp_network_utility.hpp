#ifndef UDPNETWORKUTILITY_HPP_
#define UDPNETWORKUTILITY_HPP_

#include "SDL_net/SDL_net.h"

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
