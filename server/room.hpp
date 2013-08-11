#ifndef ROOM_HPP_
#define ROOM_HPP_

class Room {
public:
	Room(/* args */);
	~Room();

	void Init();
	void Loop();
	void Quit();

	bool GetRunning() const { return running; }
private:
	bool running = true;
};

#endif
