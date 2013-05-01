#ifndef TCPNETWORKMANAGER_HPP_
#define TCPNETWORKMANAGER_HPP_

#include "SDL_net/SDL_net.h"

class TCPNetworkManager {
public:
	TCPNetworkManager();
	~TCPNetworkManager();

	void Init(int maxConnections);
	void Quit();

	int AcceptConnections();
	int CheckSockets();

	int Send(int index, const void* data, int len);
	int SendAll(const void* data, int len);

	int OpenSocket(const char* host, int port);
	TCPsocket GetSocket(int index);
	int CloseSocket(int index);

	int GetMaxConnections();
	int GetCurrentConnections();
private:
	TCPsocket serverSocket;
	SDLNet_SocketSet socketSet;
	int maxConnections, currentConnections;
};

#endif
