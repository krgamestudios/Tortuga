## Notes

The server holds the ultimate version of the world. The server has to handle log ons, logoffs, and world simulation. To do this, the server needs some sort of database system. I'd like to use something that is quick and efficient, which means that I'd probably end up using SQLite or something similar.

Monster AI/other world effects should be controlled with customizable scripts/specific file formats.

Should the server program have a UI? Or at least allow some sort of input at the terminal?

#### Template Item Information:
* item type index
* name
* effect...

#### Template Item Information:
* item index
* item type
* quantity


#### Equipment
Equipment should be items, but should the item object be placed in the equipment slot when it's equiped? Or should the slot gain certain traits, and the item is removed from the game?

#### Scratchings

* active time vs wait time battle system
* the terrain you fight on affects the strength of spells and attacks

Server Room:
* constructor
* destructor
* roomIndex
* game map
* player list
* monster list
* combat entrance list

Multithreading the network packets:
* queue
* function to push to the queue
* function to retreive from the queue
* Get the client running first


I read in an article about magic being affected by the environment. What if in the ATB system, magic was affected by what type of terrain you were fighting on? Like, if you were in a volcano, fire magic was more powerful, but ice magic was weaker?

Unless you had an ice crystal to draw magic from, which makes up for the lack of ice magic sources, but the crystals only had limited amount of uses?

-------------------------

source: http://www.gamasutra.com/blogs/EvanJones/20130701/195361/Fewer_Options_More_Meaningful_Choices.php

Compare the tension created by this dynamic to that of the lack of drama inherent in the item system of most Final Fantasy titles. In those games, characters can suffer from a variety of status impairments that last for many battles (poison, blindness, silence, etc.) Each ailment has its own respective curative item that can cure no other ailments. Because the player is not limited by inventory space, she can always carry a wide selection of curative items at all times. Status ailments never feel particularly threatening: removing them is simply a chore to be dealt with rather than an interesting decision to be made. This also reduces the impact of items that can cure any ailment - why should a player be excited to gain access to these panaceas when she’s always had the ability to easily cure any ailment? The choice of what items to bring is rendered unimportant by the large amount of available inventory space.




So I'm thinking about Boss Battles for the game. The game will have a drop in-drop out battle system, where if at least one person is still fighting the battle continues, but the people who drop out usually give something up in return (i.e. no exp, lose exp or gold, etc.) This will make permadeath easier on the players.

Anyway, for Boss battles, I want to encourage several people to take on a boss at once. one way to do this is to make the boss super strong, but I don't really want to rely on that. Another idea was to have several "minion" creatures that the boss uses.

i.e. You're fighting the Frog King, who summons four Frog Knights at the beginning of the battle. Every time a Frog Knight dies, a new one is summoned, so you're never fighting less than five creatures at once. The battle ends when the Frog King is dead, but the king never attacks you directly, instead summoning new Frog Knights, as well as healing and buffing the knights already on the field.

To defeat the Frog King, you can't just focus on him, since letting the Frog Knights attack you will almost always result in your death. So to defeat the knights and the king, the best strategy is to stun, incapacitate or draw the attention of the knights to other players, while one person attacks the king directly.

Now, although this would take a lot of mid level players to bring down, I could scale the number of knights being summoned in addition to the stats of the creatures for when there's only one or two players in the fight. With the drop in-drop out mechanic, I can scale the combat to match the number of players as well.

Anyway, this is ages away. I just wish development could go faster than it is right now. Making a video game is hard work, doubly so when you're doing it solo.

P.S. I need to add this post to the GDD.




-------------------------

## Overall Requirements

An unmodified client must be able to connect to any server, be it vanilla, custom or modded.
A server must allow the client to operate correctly.
If any information causes issues, it must be resolved.
If the source of the issue is the vanilla software, than that is the developer's responsibility to bring the software into line with the expected features.
If however, the problem is caused by modified software or assets, than it is the responsibility of the server operator to resolve the issue.

-------------------------
## Server Requirements

The server needs to handle the following content.

* server name
* server version (including host name & modification status)
* maximum player count
* logon/logoff protocol
* maintaining the client list (handling dropouts)
* persistent player data (in a database or custom format)

#### Game Resource Distribution

If the client lacks a resource, the server needs to transmit that resource to the client upon connection.
Resources transmitted include but are not limited to:

* map data
* graphical assets
	* tilesets
	* sprites
* audio assets
	* sound effects
	* background music
* scripts
	* AI scripts
	* locational triggers

#### World Maps

The maps on a server are called rooms. For the data format, See [Map File Format](wiki/Map-File-Format).
This is an example of the structure that a server's world can have. This isn't a requirement, but it can offer the easiest solutions.

* a singular "overworld", connecting different locations
* dungeons, possibly instanced
* player owned locations, like pirate ships, depending on a server's theme
* any number of possible rooms running at the same time
* each room is it's own little world

#### Game Events

This is an ambiguous section, covering several situations.

* instanced dungeons
* player combat
* timed events
* locational events

#### Active Entities

All entities on the server have a unique, unsigned index.

* players
* monsters
* combat instances
* pirate ships/vehicles
* loot drops

-------------------------
## Server Architecture

#### Modules

* server metadata
	* server name
	* version
		* distribution (official vs custom/name of the owner)
		* major.minor.patch
		* available map packs, resources, etc.
	* player count
	* maximum player count
* global client list
* global player list
* global room list
* global combat list -?

#### Client Information

* client index
* player index
* channel/address
* last contact time (ping/keep alive)
* ping count (number of times this client has been pinged, disconnect if it's
too high)

#### Player Information

* player index
* client index
* handle
* avatar
* current location (room, coords)
* inventory list
* equipment list
* friends list

#### Room Information

* room index
* map data
* player list
* monster list
* combat list
* loot drop list

#### Combat Information

* combat index
* room index
* player list
* monster list

-------------------------
## Client Requirements

The client needs to handle the following content.

* user settings (name, avatar, etc.)
* logon/logoff protocol
* resource downloading/loading
* user input
* drawing to the screen
* playing the correct information
* client index
* player index
* room data
	* player list
	* monster list
	* etc.

-------------------------
## Client Architecture

#### Scenes

* SplashScreen
	* start of the program
	* load the boilerplate resources
	* display the kr-studios logo
* MainMenu
	* program hub
* OptionScreen
	* custom config saved on exit
* Lobby
	* find available servers
		* broadcast to the network
		* phone home -?
* InWorld
	* the player is in a room
	* walk around the map
	* communicate with others on the map
* InCombat
	* the player is in combat
	* return to the original room when finished

#### Modules

* map system
* players
