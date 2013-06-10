#include "packet_type.hpp"
#include "service_locator.hpp"
#include "foobar.hpp"

#include "vector2.hpp"

#include <iostream>
using namespace std;

int main() {
	ServiceLocator<int>::Set(new int(42));
	ServiceLocator<Packet>::Set(new Packet());

	cout << FooBar() << endl;

	ServiceLocator<int>::Set(nullptr);
	ServiceLocator<Packet>::Set(nullptr);
	return 0;
}