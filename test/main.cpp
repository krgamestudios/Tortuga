//#include "packet_type.hpp"
#include "service_locator.hpp"
#include "foobar.hpp"

#include <iostream>
using namespace std;

int main() {
	ServiceLocator<int>::Set(new int(42));
	cout << FooBar() << endl;
	ServiceLocator<int>::Set(nullptr);
	return 0;
}