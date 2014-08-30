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
#ifndef SERIALPACKETBASE_HPP_
#define SERIALPACKETBASE_HPP_

#include "serial_packet_type.hpp"

#include "SDL/SDL_net.h"

//The packets use a char array for string storage
constexpr int PACKET_STRING_SIZE = 100;

class SerialPacketBase {
public:
	SerialPacketType SetType(SerialPacketType t) { return type = t; }
	SerialPacketType GetType() { return type; }

	IPaddress GetAddress() { return srcAddress; }
	IPaddress* GetAddressPtr() { return &srcAddress; }

	SerialPacketBase() {};
	virtual ~SerialPacketBase() {};

	virtual void Serialize(void* buffer) = 0;
	virtual void Deserialize(void* buffer) = 0;

protected:
	friend class UDPNetworkUtility;

	SerialPacketType type;
	IPaddress srcAddress;
};

#endif
