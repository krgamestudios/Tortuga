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
#ifndef SERVERPACKET_HPP_
#define SERVERPACKET_HPP_

#include "serial_packet_base.hpp"

#include <cstring>

class ServerPacket : public SerialPacketBase {
public:
	ServerPacket() {}
	~ServerPacket() {}

	const char* SetName(const char* s)
	{ return strncpy(name, s, PACKET_STRING_SIZE); }
	int SetPlayerCount(int i) { return playerCount = i; }
	int SetVersion(int i) { return version = i; }

	const char* GetName() { return name; }
	int GetPlayerCount() { return playerCount; }
	int GetVersion() { return version; }

	virtual void Serialize(void* buffer) override;
	virtual void Deserialize(void* buffer) override;

private:
	//identify the server
	char name[PACKET_STRING_SIZE+1];
	int playerCount;
	int version;
};

#endif