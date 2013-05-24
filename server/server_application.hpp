#ifndef SERVERAPPLICATION_HPP_
#define SERVERAPPLICATION_HPP_

#include "client_data.hpp"

#include "delta.hpp"
#include "packet_list.hpp"
#include "config_utility.hpp"
#include "udp_network_utility.hpp"

#include <map>
#include <string>

class ServerApplication {
public:
	ServerApplication() = default;
	~ServerApplication() = default;

	void Init();
	void Proc();
	void Quit();

private:
	//network commands
	void Ping(PacketData*);
	void JoinRequest(PacketData*);
	void Disconnect(int playerID);
	void Movement(PacketData*);

	void NewClientData(int playerID);
	void SendClientData(int playerID);
	void SynchronizeClient(int playerID);

	//globals
	ConfigUtility configUtil;
	UDPNetworkUtility netUtil;

	//members
	bool running = false;
	Delta delta;
	std::map<int, ClientData> clientMap;
	int maxClients = SDLNET_MAX_UDPCHANNELS;
	int uniqueIndex = 0;
};

#endif
