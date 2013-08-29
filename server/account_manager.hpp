#ifndef ACCOUNTMANAGER_HPP_
#define ACCOUNTMANAGER_HPP_

#include <list>

class AccountManager {
private:
	AccountManager() = default;
	~AccountManager() = default;
	static AccountManager instance;

public:
	static AccountManager* GetInstance() { return &instance; }

private:
	struct AccountEntry {
		int index;
	};

	std::list<AccountEntry> list;
};

#endif
