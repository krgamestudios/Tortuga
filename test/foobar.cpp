#include "service_locator.hpp"

int FooBar() {
	return *ServiceLocator<int>::Get();
}