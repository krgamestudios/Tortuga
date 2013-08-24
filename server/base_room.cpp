#include "base_room.hpp"

#include "SDL/SDL_thread.h"

#include <iostream>

BaseRoom::BaseRoom(std::map<std::string, std::string> args):
	arguments(args)
{
	//
}

int roomThread(void* ptr) {
#ifdef DEBUG
	std::cout << "Opening room" << std::endl;
#endif
	try {
		reinterpret_cast<BaseRoom*>(ptr)->Init();
		reinterpret_cast<BaseRoom*>(ptr)->Loop();
		reinterpret_cast<BaseRoom*>(ptr)->Quit();
	}
	catch(std::exception& e) {
		std::cerr << "Fatal room error: " << e.what() << std::endl;
		return 1;
	}
#ifdef DEBUG
	std::cout << "Closing room" << std::endl;
#endif
	return 0;
}
