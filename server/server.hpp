#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "delta.hpp"
#include "packet_list.hpp"

#include "config_utility.hpp"
#include "udp_network_utility.hpp"
#include "player_manager.hpp"

class Server {
public:
	Server() = default;
	~Server() = default;

	void Init();
	void Proc();
	void Quit();

private:
	void HandleInput();
	void UpdateWorld();
	void HandleOutput();

	//network commands
	void Ping(Packet*);
	void JoinRequest(Packet*);
	void Disconnect(Packet*);
	void Movement(Packet*);

	bool running = false;
	Delta delta;

	ConfigUtility configUtil;
	UDPNetworkUtility netUtil;
	PlayerManager playerMgr;

	int uniqueIndex = 0;
};

#endif
