#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "config_utility.hpp"

#include <list>

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
};

#endif
