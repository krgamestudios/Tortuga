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
#ifndef CHARACTERPACKET_HPP_
#define CHARACTERPACKET_HPP_

#include "serial_packet_base.hpp"

#include "bounding_box.hpp"
#include "vector2.hpp"

struct CharacterPacket : SerialPacketBase {
	//identify the character
	int characterIndex;
	char handle[PACKET_STRING_SIZE];
	char avatar[PACKET_STRING_SIZE];

	//the owner
	int accountIndex;

	//location
	int roomIndex;
	Vector2 origin;
	Vector2 motion;
	BoundingBox bounds;
};

void serializeCharacter(void* buffer, CharacterPacket* packet);
void deserializeCharacter(void* buffer, CharacterPacket* packet);

#endif