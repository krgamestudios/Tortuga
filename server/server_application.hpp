#ifndef SERVERAPPLICATION_HPP_
#define SERVERAPPLICATION_HPP_

#include "defines.hpp"
#include "packet_type.hpp"

#include "config_Utility.hpp"
#include "udp_network_utility.hpp"
#include "vector2.hpp"

#include <map>
#include <chrono>

class ServerApplication {
public:
	ServerApplication();
	~ServerApplication();
	void Init();
	void Proc();
	void Quit();

	ServerApplication(ServerApplication const&) = delete;
private:
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

	//game loop
	void HandleNetwork();
	void UpdateWorld();

	//network loop
	//...

	Clock::time_point lastTick = Clock::now();

	std::map<int, ClientData> clients;
	std::map<int, PlayerData> players;

	ConfigUtility configUtil;
	UDPNetworkUtility netUtil;
	bool running = false;
};

#endif
