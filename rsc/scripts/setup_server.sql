/* Copyright: (c) Kayne Ruse 2013-2016
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/

-------------------------
--defines
-------------------------

PRAGMA foreign_keys = ON;

-------------------------
--table definitions
-------------------------

CREATE TABLE IF NOT EXISTS UserAccounts (
	uid			INTEGER PRIMARY KEY AUTOINCREMENT,
	username	varchar(100) UNIQUE, --TODO: (3) Swap username for email address

	--server-client security
	passhash	varchar(100),
	passsalt	varchar(100),

	--server controls
	blacklisted	BIT DEFAULT 0,
	whitelisted	BIT DEFAULT 1,
	mod			BIT DEFAULT 0,
	admin		BIT DEFAULT 0
);

CREATE TABLE IF NOT EXISTS LiveCharacters (
	uid			INTEGER PRIMARY KEY AUTOINCREMENT,

	--metadata
	owner		INTEGER REFERENCES UserAccounts(uid),
	handle		varchar(100) UNIQUE,
	avatar		varchar(100),
	birth		timestamp NOT NULL DEFAULT (datetime()),

	--physically exists in the world
	roomIndex	INTEGER DEFAULT 0,
	originX		INTEGER DEFAULT 0,
	originY		INTEGER DEFAULT 0
);

CREATE TABLE IF NOT EXISTS DeadCharacters (
	uid INTEGER PRIMARY KEY,

	--metadata
	owner		INTEGER REFERENCES UserAccounts(uid),
	handle		varchar(100),
	avatar		varchar(100),
	birth		timestamp NOT NULL,
	death		timestamp NOT NULL DEFAULT (datetime())
);

CREATE TABLE IF NOT EXISTS LiveCreatures (
	uid INTEGER PRIMARY KEY AUTOINCREMENT,

	--metadata
	handle		varchar(100) UNIQUE,
	avatar		varchar(100),

	--actions
--	script

	--physically exists in the world
	roomIndex	INTEGER DEFAULT 0,
	originX		INTEGER DEFAULT 0,
	originY		INTEGER DEFAULT 0
);

-------------------------
--global tables
-------------------------

CREATE TABLE IF NOT EXISTS InventoryItems (
	--metadata
	uid		INTEGER PRIMARY KEY AUTOINCREMENT,
	name	varchar(100) UNIQUE,
	type	varchar(100), --tmp type

	--unique information
	durability	INTEGER DEFAULT 0
);

-------------------------
--member tables
-------------------------

CREATE TABLE IF NOT EXISTS CombatStatistics (
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
	accuracy		REAL DEFAULT 0.0,
	evasion			REAL DEFAULT 0.0,
	speed			INTEGER DEFAULT 0,
	luck			REAL DEFAULT 0.0
);

-------------------------
--cross reference tables
-------------------------

CREATE TABLE IF NOT EXISTS CharacterStatistics (
	character	INTEGER,
	statistic	INTEGER,
	FOREIGN KEY (character)	REFERENCES LiveCharacters(uid),
	FOREIGN KEY (statistic)	REFERENCES CombatStatistics(uid)
);

CREATE TABLE IF NOT EXISTS CharacterItems (
	character	INTEGER,
	item		INTEGER,
	FOREIGN KEY (character)	REFERENCES LiveCharacters(uid),
	FOREIGN KEY (item)		REFERENCES InventoryItem(uid)
);

CREATE TABLE IF NOT EXISTS CharacterEquipment (
	character	INTEGER,
	item		INTEGER,
	FOREIGN KEY (character)	REFERENCES LiveCharacters(uid),
	FOREIGN KEY (item)		REFERENCES InventoryItem(uid)
);

