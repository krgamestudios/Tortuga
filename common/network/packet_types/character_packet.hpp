/* Copyright: (c) Kayne Ruse 2013, 2014
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
#ifndef CHARACTERPACKET_HPP_
#define CHARACTERPACKET_HPP_

#include "serial_packet_base.hpp"

#include "vector2.hpp"
#include "statistics.hpp"

#include <cstring>

class CharacterPacket : public SerialPacketBase {
public:
	CharacterPacket() {}
	~CharacterPacket() {}

	//indentity
	int SetCharacterIndex(int i) { return characterIndex = i; }
	const char* SetHandle(const char* s)
	{ return strncpy(handle, s, PACKET_STRING_SIZE); }
	const char* SetAvatar(const char* s)
	{ return strncpy(handle, s, PACKET_STRING_SIZE); }

	int SetAccountIndex(int i) { return accountIndex = i; }

	int GetCharacterIndex() { return characterIndex; }
	const char* GetHandle() { return handle; }
	const char* GetAvatar() { return avatar; }

	int GetAccountIndex() { return accountIndex; }

	//location
	int SetRoomIndex(int i) { return roomIndex = i; }
	Vector2 SetOrigin(Vector2 v) { return origin = v; }
	Vector2 SetMotion(Vector2 v) { return motion = v; }

	int GetRoomIndex() { return roomIndex; }
	Vector2 GetOrigin() { return origin; }
	Vector2 GetMotion() { return motion; }

	//gameplay
	Statistics* GetStatistics() { return &stats; }

	virtual void Serialize(void* buffer) override;
	virtual void Deserialize(void* buffer) override;

private:
	//identify the character
	int characterIndex;
	char handle[PACKET_STRING_SIZE+1];
	char avatar[PACKET_STRING_SIZE+1];

	//the owner
	int accountIndex;

	//location
	int roomIndex;
	Vector2 origin;
	Vector2 motion;

	//gameplay
	Statistics stats;

	//TODO: gameplay components: equipment, items, buffs, debuffs
};

#endif