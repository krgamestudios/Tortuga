#ifndef BASEROOM_HPP_
#define BASEROOM_HPP_

#include "mail_box.hpp"

#include <map>
#include <string>

//The abstract base class for all rooms
class BaseRoom {
public:
	BaseRoom() = default;
	~BaseRoom() = default;

	virtual void Init() = 0;
	virtual void Loop() = 0;
	virtual void Quit() = 0;

	bool SetRunning(bool b) { return running = b; }
	bool GetRunning() const { return running; }

	MailBox* GetMailBox() { return& mailBox; }

protected:
	MailBox mailBox;

private:
	bool running = true;
};

#endif
