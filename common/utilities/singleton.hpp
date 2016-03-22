/* Copyright: (c) Kayne Ruse 2013-2016
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
#pragma once

#include <stdexcept>

template<typename T>
class Singleton {
public:
	static T& GetSingleton() {
		if (!ptr) {
			throw(std::logic_error("This singleton has not been created"));
		}
		return *ptr;
	}
	static void CreateSingleton() {
		if (ptr) {
			throw(std::logic_error("This singleton has already been created"));
		}
		ptr = new T();
	}
	static void DeleteSingleton() {
		if (!ptr) {
			throw(std::logic_error("A non-existant singleton cannot be deleted"));
		}
		delete ptr;
		ptr = nullptr;
	}

protected:
	Singleton() = default;
	Singleton(Singleton const&) = default;
	Singleton(Singleton&&) = default;
	~Singleton() = default;

private:
	static T* ptr;
};

template<typename T>
T* Singleton<T>::ptr = nullptr;
