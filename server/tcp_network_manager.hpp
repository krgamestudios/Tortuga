#ifndef TCPNETWORKMANAGER_HPP_
#define TCPNETWORKMANAGER_HPP_

#include "SDL_net/SDL_net.h"

class TCPNetworkManager {
public:
	TCPNetworkManager();
	~TCPNetworkManager();

	void Init(Uint16 port, int maxSockets);
	void Quit();

	int AcceptConnections();
	int CheckSockets();

	int Send(int index, const void* data, int len);
	int SendAll(const void* data, int len);

	int OpenSocket(const char* host, int port);
	TCPsocket GetSocket(int index);
	int CloseSocket(int index);

	int GetMaxConnections() const;
	int GetCurrentConnections() const;
private:
	TCPsocket sock;
	SDLNet_SocketSet clientSocks;
};

#endif
