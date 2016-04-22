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
--fill the global tables with gameplay data
-------------------------

INSERT OR IGNORE INTO InventoryItems (name, type, durability) VALUES 
		("sword", "weapon", 100),
		("dagger", "weapon", 100),
		("staff", "weapon", 100),
		("shield", "armour", 100),
		("potion", "consumable", 100)
;

--DEBUG: Test cases
INSERT INTO LiveCharacters

--DEBUG: this is supposed to archive the dead characters
--Insert into DeadCharacters From LiveCharacters all characters who's HP has reached zero or below
INSERT INTO DeadCharacters (uid, owner, handle, avatar, birth)
	SELECT uid, owner, handle, avatar, birth FROM LiveCharacters WHERE
		SELECT character FROM CharacterStatistics WHERE
			SELECT uid FROM CombatStatistics WHERE health <= 0;

DELETE FROM LiveCharacters WHERE uid IN (SELECT uid FROM DeadCharacters);