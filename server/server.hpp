#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "udp_network_utility.hpp"
#include "config_utility.hpp"

class Server {
public:
	Server() = default;
	~Server() = default;

	void Init();
	void Proc();
	void Quit();

	void HandleInput();
	void UpdateWorld();
	void HandleOutput();
private:
	bool running = false;
	ConfigUtility config;
	UDPNetworkUtility netUtil;
};

#endif
