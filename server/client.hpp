#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "SDL_net/SDL_net.h"

class Client {
public:
	Client(int id, TCPsocket);

	int GetID() const {
		return id;
	}
	TCPsocket GetSocket() const {
		return socket;
	}
private:
	int id;
	TCPsocket socket;
};

#endif
