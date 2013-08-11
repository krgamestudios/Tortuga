#include "application.hpp"

#include "SDL/SDL.h"

#include <stdexcept>
#include <iostream>

using namespace std;

int main(int, char**) {
#ifdef DEBUG
	cout << "Beginning server" << endl;
#endif
	try {
		Application app;
		app.Init();
		app.Loop();
		app.Quit();
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
