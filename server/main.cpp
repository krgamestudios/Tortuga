#include "server_application.hpp"

#include "SDL/SDL.h"

#include <stdexcept>
#include <iostream>

using namespace std;

int main(int, char**) {
#ifdef DEBUG
	cout << "Beginning server" << endl;
#endif
	try {
		ServerApplication::GetInstance()->Init();
		ServerApplication::GetInstance()->Loop();
		ServerApplication::GetInstance()->Quit();
	}
	catch(exception& e) {
		cerr << "Fatal error: " << e.what() << endl;
		return 1;
	}
#ifdef DEBUG
	cout << "Clean exit" << endl;
#endif
	return 0;
}
