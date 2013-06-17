#ifndef NETWORKQUEUE_HPP_
#define NETWORKQUEUE_HPP_

#include "packet_type.hpp"

void BeginQueueThread();
void EndQueueThread();
void KillQueueThread();
Packet peekNetworkPacket();
Packet popNetworkPacket();

#endif
