/* Copyright: (c) Kayne Ruse 2013-2015
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

--TODO: might need a version number for database structures

--An example of a database update script.
--This is only rough right now, until I get some more eyes onto it

--moving old to new
CREATE TABLE tempCharacters (
	uid			INTEGER PRIMARY KEY AUTOINCREMENT,

	--metadata
	owner		INTEGER REFERENCES Accounts(uid),
	handle		varchar(100) UNIQUE,
	avatar		varchar(100),
	birth		timestamp NOT NULL DEFAULT (datetime()),

	--physically exists in the world
	roomIndex	INTEGER DEFAULT 0,
	originX		INTEGER DEFAULT 0,
	originY		INTEGER DEFAULT 0,
	boundsX		INTEGER DEFAULT 0,
	boundsY		INTEGER DEFAULT 0,
	boundsW		INTEGER DEFAULT 0,
	boundsH		INTEGER DEFAULT 0
);

INSERT INTO tempCharacters (uid, owner, handle, avatar, birth, roomIndex, originX, originY, boundsX, boundsY, boundsW, boundsH)
	SELECT uid, owner, handle, avatar, birth, roomIndex, originX, originY, boundsX, boundsY, boundsW, boundsH
	FROM LiveCharacters;

DROP TABLE LiveCharacters;

ALTER TABLE tempCharacters RENAME TO LiveCharacters;
