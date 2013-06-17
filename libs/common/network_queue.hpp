#ifndef NETWORKQUEUE_HPP_
#define NETWORKQUEUE_HPP_

#include "packet_type.hpp"

int networkQueue(void*);
Packet peekNetworkPacket();
Packet popNetworkPacket();

#endif
