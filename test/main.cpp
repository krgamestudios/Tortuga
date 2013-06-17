#include "packet_type.hpp"

#include <iostream>

using namespace std;

int main(int, char**) {
	Packet p;
	cout << int(p.meta.type) << endl;
	return 0;
}