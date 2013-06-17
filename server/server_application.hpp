#ifndef SERVERAPPLICATION_HPP_
#define SERVERAPPLICATION_HPP_

#include "defines.hpp"
#include "packet_type.hpp"
#include "service_locator.hpp"
#include "network_queue.hpp"

#include "config_Utility.hpp"
#include "udp_network_utility.hpp"
#include "vector2.hpp"

#include "SDL/SDL_thread.h"

#include <map>
#include <chrono>

struct ClientData {
	int index;
	int channel;
	int playerIndex;
};

struct PlayerData {
	int index;
	int clientIndex;
	Vector2 position;
	Vector2 motion;

	void Update(double delta) {
		position += motion * delta;
	}
};

class ServerApplication {
public:
	ServerApplication();
	~ServerApplication();
	void Init();
	void Proc();
	void Quit();

	ServerApplication(ServerApplication const&) = delete;
private:
	//game loop
	void UpdateWorld(double delta);

	//network loop
	int HandlePacket(Packet p);
	void Broadcast(BroadcastRequest&);

	//services
	ConfigUtility* configUtil = nullptr;
	UDPNetworkUtility* netUtil = nullptr;

	//members
	Clock::time_point lastTick = Clock::now();

	std::map<int, ClientData> clients;
	std::map<int, PlayerData> players;

	bool running = false;

	//threads
	SDL_Thread* queueThread = nullptr;
};

#endif
