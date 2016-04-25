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
#include "heartbeat_utility.hpp"

#include "channels.hpp"
#include "ip_operators.hpp"

//heartbeat system
void HeartbeatUtility::hPing(ServerPacket* const argPacket) {
	ServerPacket newPacket;
	newPacket.type = SerialPacketType::PONG;
	network.SendTo(argPacket->srcAddress, &newPacket);
}

void HeartbeatUtility::hPong(ServerPacket* const argPacket) {
	if (*network.GetIPAddress(Channels::SERVER) != argPacket->srcAddress) {
		throw(std::runtime_error("Heartbeat message received from an unknown source"));
	}
	attemptedBeats = 0;
	lastBeat = Clock::now();
}

int HeartbeatUtility::CheckHeartBeat() {
	//check the connection (heartbeat)
	if (Clock::now() - lastBeat > std::chrono::seconds(3)) {
		if (attemptedBeats > 2) {
			return 1;
		}
		else {
			ServerPacket newPacket;
			newPacket.type = SerialPacketType::PING;
			network.SendTo(Channels::SERVER, &newPacket);

			attemptedBeats++;
			lastBeat = Clock::now();
		}
	}
	return 0;
}
