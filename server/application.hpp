#ifndef APPLICATION_HPP_
#define APPLICATION_HPP_

#include "threading.hpp"

#include <vector>

class Application {
public:
	Application();
	~Application();

	void Init();
	void Loop();
	void Quit();

	bool GetRunning() const { return running; }
private:
	void NewRoom(/* args */);

	std::vector<roomHandle> rooms;
	bool running = true;
};

#endif
