#include "threading.hpp"

#include <stdexcept>
#include <iostream>

int roomThread(void* ptr) {
#ifdef DEBUG
	std::cout << "Opening room" << std::endl;
#endif
	try {
		reinterpret_cast<Room*>(ptr)->Init();
		reinterpret_cast<Room*>(ptr)->Loop();
		reinterpret_cast<Room*>(ptr)->Quit();
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
