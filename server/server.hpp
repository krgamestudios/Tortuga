#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "tcp_network_manager.hpp"
#include "config_utility.hpp"

class Server {
public:
	Server();
	~Server();

	void Init();
	void Proc();
	void Quit();

	void HandleInput();
	void UpdateWorld();
	void HandleOutput();
private:
	bool running;
	TCPNetworkManager netMgr;
	ConfigUtility config;
};

#endif
