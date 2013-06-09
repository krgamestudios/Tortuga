#ifndef SERVICELOCATOR_HPP_
#define SERVICELOCATOR_HPP_

template<typename T>
class ServiceLocator {
public:
	static T* Set(T* t) {
		delete service;
		return service = t;
	}
	static T* Get() {
		return service;
	}
private:
	static T* service;
};

template<typename T>
T* ServiceLocator<T>::service = nullptr;

#endif
