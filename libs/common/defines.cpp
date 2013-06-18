#include "defines.hpp"

#include <cstdio>

std::string itos(int i) {
	char buffer[20];
	snprintf(buffer, 20, "%d", i);
	return std::string(buffer);
}