#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

/*
template<typename T>
class Singleton {
public:
	static T* GetSingletonPtr() {
		return &singleton;
	}
	static T& GetSingletonRef() {
		return singleton;
	}
private:
	Singleton();
	~Singleton();
	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);
	Singleton(Singleton&&);
	Singleton& operator=(Singleton&&);
	static T singleton;
};
*/

template<typename T>
T* GetSingletonPtr() {
	static T t;
	return &t;
}

#endif
