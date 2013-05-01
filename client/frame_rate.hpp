#ifndef FRAMERATE_HPP_
#define FRAMERATE_HPP_

#include <ctime>

class FrameRate {
public:
	FrameRate() {
		frameCount = lastFrameRate = tick = 0;
	}
	int Calculate() {
		frameCount++;
		if (clock() - tick >= CLOCKS_PER_SEC) {
			lastFrameRate = frameCount;
			frameCount = 0;
			tick = clock();
		}
		return lastFrameRate;
	}
	int GetFrameRate() {
		return lastFrameRate;
	}
private:
	int frameCount;
	int lastFrameRate;
	int tick;
};

#endif
