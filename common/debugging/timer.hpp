/* Copyright: (c) Kayne Ruse 2013-2015
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <chrono>
#include <string>
#include <ostream>

class Timer {
public:
	typedef std::chrono::high_resolution_clock Clock;

	Timer();
	Timer(std::string s);
	~Timer() = default;

	void Start();
	void Stop();

	//accessors and mutators
	Clock::duration GetTime() { return timeSpan; }

	std::string SetName(std::string s) { return name = s; }
	std::string GetName() { return name; }

private:
	std::string name;
	Clock::time_point start;
	Clock::duration timeSpan;
};

std::ostream& operator<<(std::ostream& os, Timer& t);

#endif
