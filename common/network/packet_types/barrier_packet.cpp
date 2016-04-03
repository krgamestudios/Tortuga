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
#include "barrier_packet.hpp"

#include "serial_utility.hpp"

void serializeBarrier(void* buffer, BarrierPacket* packet) {
	serialCopy(&buffer, &packet->type, sizeof(SerialPacketType));

	//identify the barrier
	serialCopy(&buffer, &packet->barrierIndex, sizeof(int));

	//bounds
	serialCopy(&buffer, &packet->bounds.x, sizeof(int));
	serialCopy(&buffer, &packet->bounds.y, sizeof(int));
	serialCopy(&buffer, &packet->bounds.w, sizeof(int));
	serialCopy(&buffer, &packet->bounds.h, sizeof(int));


	//location
	serialCopy(&buffer, &packet->roomIndex, sizeof(int));
	serialCopy(&buffer, &packet->origin.x, sizeof(double));
	serialCopy(&buffer, &packet->origin.y, sizeof(double));
	serialCopy(&buffer, &packet->motion.x, sizeof(double));
	serialCopy(&buffer, &packet->motion.y, sizeof(double));

	//graphical data
	for (int i = 0; i < 8; i++) {
		serialCopy(&buffer, &packet->status[i], sizeof(int));
	}
}

void deserializeBarrier(void* buffer, BarrierPacket* packet) {
	deserialCopy(&buffer, &packet->type, sizeof(SerialPacketType));

	//identify the barrier
	deserialCopy(&buffer, &packet->barrierIndex, sizeof(int));

	//bounds
	deserialCopy(&buffer, &packet->bounds.x, sizeof(int));
	deserialCopy(&buffer, &packet->bounds.y, sizeof(int));
	deserialCopy(&buffer, &packet->bounds.w, sizeof(int));
	deserialCopy(&buffer, &packet->bounds.h, sizeof(int));


	//location
	deserialCopy(&buffer, &packet->roomIndex, sizeof(int));
	deserialCopy(&buffer, &packet->origin.x, sizeof(double));
	deserialCopy(&buffer, &packet->origin.y, sizeof(double));
	deserialCopy(&buffer, &packet->motion.x, sizeof(double));
	deserialCopy(&buffer, &packet->motion.y, sizeof(double));

	//graphical data
	for (int i = 0; i < 8; i++) {
		deserialCopy(&buffer, &packet->status[i], sizeof(int));
	}
}
