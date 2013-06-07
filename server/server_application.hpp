#ifndef SERVERAPPLICATION_HPP_
#define SERVERAPPLICATION_HPP_

class ServerApplication {
public:
	ServerApplication();
	~ServerApplication();
	void Init();
	void Proc();
	void Quit();
private:
	bool running = true;
};

#endif
