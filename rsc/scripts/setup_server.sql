-------------------------
--Server
-------------------------

CREATE TABLE IF NOT EXISTS UserAccounts (
	uid INTEGER PRIMARY KEY AUTOINCREMENT,
	username varchar(100) UNIQUE,
	--TODO: server-client security
--	password varchar(100),
	blacklisted BIT DEFAULT 0,
	whitelisted BIT DEFAULT 1
--	TODO: moderator
);

-------------------------
--Items
-------------------------

CREATE TABLE IF NOT EXISTS MundaneItems (
	--metadata
	uid INTEGER PRIMARY KEY AUTOINCREMENT,
	itemID INTEGER,
	stackSize INTEGER DEFAULT 0,
	owner INTEGER REFERENCES PlayerCharacters(uid)
);

CREATE TABLE IF NOT EXISTS Consumables (
	--metadata
	uid INTEGER PRIMARY KEY AUTOINCREMENT,
	itemID INTEGER,
	stackSize INTEGER DEFAULT 0,
	owner INTEGER REFERENCES PlayerCharacters(uid)
	--holds all consumable items info (food, potions, etc.)
);

CREATE TABLE IF NOT EXISTS Equipment (
	--metadata
	uid INTEGER PRIMARY KEY AUTOINCREMENT,
	itemID INTEGER,
	owner INTEGER REFERENCES PlayerCharacters(uid)
	--hold all equipment info
	--stat mods, special effects, etc.
);

-------------------------
--Players
-------------------------

CREATE TABLE IF NOT EXISTS PlayerCharacters (
	uid INTEGER PRIMARY KEY AUTOINCREMENT,

	--metadata
	owner INTEGER REFERENCES UserAccounts(uid),
	handle varchar(100) UNIQUE,
	avatar varchar(100),
	birth timestamp NOT NULL DEFAULT (datetime()),

	--position
	mapIndex		INTEGER DEFAULT 0,
	positionX		INTEGER DEFAULT 0,
	positionY		INTEGER DEFAULT 0,

	--statistics
	level			INTEGER DEFAULT 0,
	exp				INTEGER DEFAULT 0,
	maxHP			INTEGER DEFAULT 0,
	health			INTEGER DEFAULT 0,
	maxMP			INTEGER DEFAULT 0,
	mana			INTEGER DEFAULT 0,
	attack			INTEGER DEFAULT 0,
	defence			INTEGER DEFAULT 0,
	intelligence	INTEGER DEFAULT 0,
	resistance		INTEGER DEFAULT 0,
	speed			INTEGER DEFAULT 0,
	accuracy		REAL DEFAULT 0.0,
	evasion			REAL DEFAULT 0.0,
	luck			REAL DEFAULT 0.0,

	--equipment
	weapon INTEGER REFERENCES Equipment(uid),
	helmet INTEGER REFERENCES Equipment(uid),
	armour INTEGER REFERENCES Equipment(uid)
	--etc.
);
