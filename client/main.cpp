#include "scene_manager.h"

#include <stdexcept>
#include <iostream>

using namespace std;

int main(int, char**) {
#ifdef DEBUG
	cout << "Beginning program" << endl;
#endif
	SceneManager app;
	try {
		app.Init();
		app.Proc();
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
