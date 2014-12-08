/* Copyright: (c) Kayne Ruse 2014
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
#include "server_application.hpp"

//-------------------------
//these should've come standard
//-------------------------

bool operator==(IPaddress lhs, IPaddress rhs) {
	return lhs.host == rhs.host && lhs.port == rhs.port;
}

bool operator!=(IPaddress lhs, IPaddress rhs) {
	return !(lhs == rhs);
}

//-------------------------
//Packet pumps
//-------------------------

void ServerApplication::PumpPacket(SerialPacket* const argPacket) {
	for (auto& it : *clientMgr.GetContainer()) {
		network.SendTo(it.second.GetAddress(), argPacket);
	}
}

void ServerApplication::PumpPacketProximity(SerialPacket* const argPacket, int roomIndex, int x, int y, int radius) {
	//TODO: PumpPacketProximity
	//for position (roomIndex, x, y), find all characters within that distance
	//find that character's owner
	//find that account's client
	//send the packet to that client
	//NOTE: this is perhaps too complex; I write it if I need it
}
