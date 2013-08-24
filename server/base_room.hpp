#ifndef BASEROOM_HPP_
#define BASEROOM_HPP_

#include "mail_box.hpp"

#include <map>
#include <string>

class BaseRoom {
public:
	BaseRoom(std::map<std::string, std::string> args);
	~BaseRoom() = default;

	virtual void Init() = 0;
	virtual void Loop() = 0;
	virtual void Quit() = 0;

	bool SetRunning(bool b) { return running = b; }
	bool GetRunning() const { return running; }

	MailBox* GetMailBox() { return& mailBox; }

protected:
	std::map<std::string, std::string> const arguments;
	MailBox mailBox;

private:
	bool running = true;
};

int roomThread(void*);

#endif
