#include "network.hpp"

#include <iostream>

using namespace std;

//receive any amount of info and print it

int main(int, char**) {
	NetworkInit();
	TCPSocket sock("127.0.0.1",2000);
	char buffer[512];

	while(true) {
		memset(buffer, 0, 512);
		if (sock.Recv(buffer, 512)) {
			cout << buffer << endl;
		}
	}
	NetworkQuit();
	return 0;
}