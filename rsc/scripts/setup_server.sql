CREATE TABLE IF NOT EXISTS Accounts (
	uid INTEGER PRIMARY KEY AUTOINCREMENT,
	username varchar(100) UNIQUE,
	--TODO: server-client security
--	password varchar(100),
	blacklisted BIT DEFAULT 0,
	whitelisted BIT DEFAULT 1,
	administrator BIT DEFAULT 0
);

CREATE TABLE IF NOT EXISTS Characters (
	uid INTEGER PRIMARY KEY AUTOINCREMENT,

	--metadata
	owner INTEGER REFERENCES Accounts(uid),
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
	weapon INTEGER REFERENCES WornEquipment(uid),
	helmet INTEGER REFERENCES WornEquipment(uid),
	armour INTEGER REFERENCES WornEquipment(uid)
	--etc.
);

CREATE TABLE IF NOT EXISTS InventoryItems (
	--metadata
	uid INTEGER PRIMARY KEY AUTOINCREMENT,
	itemID INTEGER, --type
	stackSize INTEGER DEFAULT 0,
	owner INTEGER REFERENCES Characters(uid)
);

CREATE TABLE IF NOT EXISTS WornEquipment (
	--metadata
	uid INTEGER PRIMARY KEY AUTOINCREMENT,
	itemID INTEGER, --type
	owner INTEGER REFERENCES Characters(uid)
	--hold all equipment info
	--stat mods, special effects, etc.
);