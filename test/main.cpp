#include "vector2.hpp"

#include <iostream>

using namespace std;

int main(int, char**) {
	Vector2 v = {1, 1};
	Vector2 a = 99 * v;
	cout << a.x << ", " << a.y << endl;
	return 0;
}