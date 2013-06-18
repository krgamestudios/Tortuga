#ifndef NETWORKQUEUE_HPP_
#define NETWORKQUEUE_HPP_

#include "packet_type.hpp"

void beginQueueThread();
void endQueueThread();
void killQueueThread();
Packet peekNetworkPacket();
Packet popNetworkPacket();
void flushNetworkQueue();

#endif
