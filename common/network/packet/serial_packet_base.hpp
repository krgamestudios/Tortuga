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

#ifndef SERIALPACKET_HPP_
 #error Cannot include this file without 'serial_packet.hpp'
#endif

#include "serial_packet_type.hpp"

#include "SDL/SDL_net.h"

#define NETWORK_VERSION 20140607
#define PACKET_STRING_SIZE 100

struct SerialPacketBase {
	//members
	SerialPacketType type;
	IPaddress srcAddress;

	virtual ~SerialPacketBase() {};
};

typedef SerialPacketBase SerialPacket;

#endif
