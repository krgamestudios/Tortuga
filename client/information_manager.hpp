#ifndef INFORMATIONMANAGER_HPP_
#define INFORMATIONMANAGER_HPP_

class InformationManager {
public:
	int SetClientIndex(int i) { return clientIndex = i; }
	int GetClientIndex() { return clientIndex; }
	void ResetClientIndex() { clientIndex = -1; }
private:
	int clientIndex = -1;
};

#endif
