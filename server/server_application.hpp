#ifndef SERVERAPPLICATION_HPP_
#define SERVERAPPLICATION_HPP_

#include "base_room.hpp"

#include "SDL/SDL_thread.h"

#include <list>
#include <map>
#include <string>

struct roomHandle {
	SDL_Thread* thread = nullptr;
	BaseRoom* room = nullptr;
};

class ServerApplication {
public:
	ServerApplication();
	~ServerApplication();

	void Init();
	void Loop();
	void Quit();

	bool SetRunning(bool b) { return running = b; }
	bool GetRunning() const { return running; }

private:
	void OpenRoom(std::map<std::string, std::string>);
	void CloseRoom(roomHandle);

	std::list<roomHandle> rooms;
	bool running = true;
};

#endif
