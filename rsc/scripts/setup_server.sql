--TODO: An archive table of all dead characters

CREATE TABLE IF NOT EXISTS Accounts (
	uid			INTEGER PRIMARY KEY AUTOINCREMENT,
	username	varchar(100) UNIQUE,

	--TODO: server-client security
--	passhash	varchar(100),
--	passsalt	varchar(100),

	--server controls
	blacklisted	BIT DEFAULT 0,
	whitelisted	BIT DEFAULT 1,
	mod			BIT DEFAULT 0,
	admin		BIT DEFAULT 0
);

CREATE TABLE IF NOT EXISTS Characters (
	uid			INTEGER PRIMARY KEY AUTOINCREMENT,

	--metadata
	owner		INTEGER REFERENCES Accounts(uid),
	handle		varchar(100) UNIQUE,
	avatar		varchar(100),
	birth		timestamp NOT NULL DEFAULT (datetime()),

	--position in the world
	roomIndex	INTEGER DEFAULT 0,
	originX		INTEGER DEFAULT 0,
	originY		INTEGER DEFAULT 0,

	--statistics
	baseStats	INTEGER REFERENCES StatisticSets(uid),

	--equipment
	weapon		INTEGER REFERENCES WornEquipment(uid),
	helmet		INTEGER REFERENCES WornEquipment(uid),
	armour		INTEGER REFERENCES WornEquipment(uid)
	--etc.
);

-------------------------
--Utility tables
-------------------------

CREATE TABLE IF NOT EXISTS StatisticSets (
	--metadata
	uid				INTEGER PRIMARY KEY AUTOINCREMENT,

	--general use statistics
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
	luck			REAL DEFAULT 0.0
);

CREATE TABLE IF NOT EXISTS InWorldItems (
	--metadata
	uid			INTEGER PRIMARY KEY AUTOINCREMENT,
	itemType	INTEGER,

	--position in the world
	roomIndex	INTEGER DEFAULT 0,
	originX		INTEGER DEFAULT 0,
	originY		INTEGER DEFAULT 0,

	--unique information
	stackSize	INTEGER DEFAULT 0,
	durability	INTEGER DEFAULT 0,
	stats		INTEGER REFERENCES StatisticSets(uid)
);

CREATE TABLE IF NOT EXISTS InventoryItems (
	--metadata
	uid			INTEGER PRIMARY KEY AUTOINCREMENT,
	owner		INTEGER REFERENCES Characters(uid),
	itemType	INTEGER,

	--unique information
	stackSize	INTEGER DEFAULT 0,
	durability	INTEGER DEFAULT 0,
	stats		INTEGER REFERENCES StatisticSets(uid)
);

CREATE TABLE IF NOT EXISTS WornEquipment (
	--metadata
	uid			INTEGER PRIMARY KEY AUTOINCREMENT,
	owner		INTEGER REFERENCES Characters(uid),
	itemType	INTEGER,

	--unique information
	durability	INTEGER DEFAULT 0,
	stats		INTEGER REFERENCES StatisticSets(uid)
	--TODO: attached script?
);
