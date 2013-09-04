-------------------------
--Server
-------------------------

CREATE TABLE IF NOT EXISTS UserAccounts (
	userAccountID INTEGER PRIMARY KEY AUTOINCREMENT,
	username varchar(30) UNIQUE,
	password varchar(30),
	blacklisted BIT DEFAULT 0,
	whitelisted BIT DEFAULT 1
);

-------------------------
--Items
-------------------------

CREATE TABLE IF NOT EXISTS GlobalItemList (
	globalItemListID INTEGER PRIMARY KEY AUTOINCREMENT,
	itemName varchar(30) UNIQUE,
	itemImage varchar(30),
	type varchar(15), --{'mundane', 'consumable', 'equipment'}
	maxStackSize INTEGER, --{1-max; 0 for non-stackable}
	maxUniqueCopies INTEGER --{1-max; 0 for unlimited}
);

CREATE TABLE IF NOT EXISTS MundaneItems (
	mundaneItemID INTEGER PRIMARY KEY AUTOINCREMENT,
	globalItemListID INTEGER REFERENCES GlobalItemList(globalItemListID)
	--holds whatever
);

CREATE TABLE IF NOT EXISTS Consumables (
	consumableID INTEGER PRIMARY KEY AUTOINCREMENT,
	globalItemListID INTEGER REFERENCES GlobalItemList(globalItemListID)
	--holds all consumable items info (food, potions, etc.)
);

CREATE TABLE IF NOT EXISTS Equipment (
	equipmentID INTEGER PRIMARY KEY AUTOINCREMENT,
	globalItemListID INTEGER REFERENCES GlobalItemList(globalItemListID)
	--hold all equipment info
);

-------------------------
--Players
-------------------------

CREATE TABLE IF NOT EXISTS PlayerCharacters (
	playerCharacterID INTEGER PRIMARY KEY AUTOINCREMENT,
	name varchar(30) UNIQUE,

	--stats
	currentLevel		INTEGER DEFAULT 0,
	currentExperience	INTEGER DEFAULT 0,
	maxHealth			INTEGER DEFAULT 0,
	maxMana				INTEGER DEFAULT 0,
	currentHealth		INTEGER DEFAULT 0,
	currentMana			INTEGER DEFAULT 0,
	attack				INTEGER DEFAULT 0,
	defence				INTEGER DEFAULT 0,
	--etc.

	--equipment
	weapon INTEGER REFERENCES Equipment(equipmentID),
	helmet INTEGER REFERENCES Equipment(equipmentID),
	armour INTEGER REFERENCES Equipment(equipmentID)
	--etc.
);

CREATE TABLE IF NOT EXISTS PlayerInventoryItems (
	characterID INTEGER REFERENCES PlayerCharacters(characterID),
	globalItemListID INTEGER REFERENCES GlobalItemList(globalItemListID)
);

--cleanup
DROP TABLE UserAccounts;
DROP TABLE GlobalItemList;
DROP TABLE MundaneItems;
DROP TABLE Consumables;
DROP TABLE Equipment;
DROP TABLE PlayerCharacters;
DROP TABLE PlayerInventoryItems;