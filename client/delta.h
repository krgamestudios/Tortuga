#ifndef DELTA_H_
#define DELTA_H_

#include <ctime>

class Delta {
public:
	Delta() {
		time = tick = 0;
	}
	int Calculate() {
		int c = clock();
		time = c - tick;
		tick = c;
		return time;
	}
	int GetDelta() const {
		return time;
	};
private:
	int time, tick;
};

#endif
