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
#ifndef SERIAL_HPP_
#define SERIAL_HPP_

#include "serial_packet.hpp"

/* NOTE: Keep the PACKET_BUFFER_SIZE up to date
 * NOTE: REGION_CONTENT is currently the largest type of packet
 *  map content: REGION_WIDTH * REGION_HEIGHT * REGION_DEPTH * sizoeof(region::type_t)
 *  map format: sizeof(int) * 2
 *  metadata: sizeof(metadata)
*/
#define PACKET_BUFFER_SIZE REGION_WIDTH * REGION_HEIGHT * REGION_DEPTH * sizeof(Region::type_t) + sizeof(int) * 2 + sizeof(SerialPacket::Metadata)

void serialize(SerialPacket* const, void*);
void deserialize(SerialPacket* const, void*);

#endif
