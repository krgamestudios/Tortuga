#ifndef SERVERAPPLICATION_HPP_
#define SERVERAPPLICATION_HPP_

//The main application class
class ServerApplication {
private:
	ServerApplication();
	~ServerApplication();
	static ServerApplication instance;

public:
	static ServerApplication* GetInstance() { return &instance; }

	void Init();
	void Loop();
	void Quit();

private:
	bool running = true;
};

#endif
