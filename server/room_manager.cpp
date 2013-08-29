#include "room_manager.hpp"

#include <iostream>

RoomManager RoomManager::instance;

int roomThread(void* ptr) {
	std::cout << "Opening room" << std::endl;
	try {
		reinterpret_cast<BaseRoom*>(ptr)->Init();
		reinterpret_cast<BaseRoom*>(ptr)->Loop();
		reinterpret_cast<BaseRoom*>(ptr)->Quit();
	}
	catch(std::exception& e) {
		std::cerr << "Fatal room error: " << e.what() << std::endl;
		return 1;
	}
	std::cout << "Closing room" << std::endl;
	return 0;
}

