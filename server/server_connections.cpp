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
#include "server_application.hpp"

#include <iostream>
#include <sstream>

//-------------------------
//heartbeat system
//-------------------------

void ServerApplication::hPing(ServerPacket* const argPacket) {
	ServerPacket newPacket;
	newPacket.type = SerialPacketType::PONG;
	network.SendTo(argPacket->srcAddress, &newPacket);
}

void ServerApplication::hPong(ServerPacket* const argPacket) {
	clientMgr.HandlePong(argPacket);
}

//-------------------------
//basic connections
//-------------------------

void ServerApplication::hBroadcastRequest(ServerPacket* const argPacket) {
	//send the server's data
	ServerPacket newPacket;

	newPacket.type = SerialPacketType::BROADCAST_RESPONSE;
	strncpy(newPacket.name, config["server.name"].c_str(), PACKET_STRING_SIZE);
	newPacket.playerCount = characterMgr.GetLoadedCount();
	newPacket.version = NETWORK_VERSION;

	network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));
}

void ServerApplication::hJoinRequest(ClientPacket* const argPacket) {
	//register the client
	int clientIndex = clientMgr.Create(argPacket->srcAddress);

	//send the client their info
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::JOIN_RESPONSE;
	newPacket.clientIndex = clientIndex;

	network.SendTo(argPacket->srcAddress, static_cast<SerialPacket*>(&newPacket));

	//finished this routine
	std::cout << "New join, " << clientMgr.GetLoadedCount() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

void ServerApplication::hLoginRequest(ClientPacket* const argPacket) {
	//get the client data
	ClientData* clientData = clientMgr.Get(argPacket->clientIndex);

	if (clientData == nullptr || clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified client index detected: " << argPacket->clientIndex << std::endl;
		return;
	}

	//load the user account
	int accountIndex = accountMgr.Load(argPacket->username, argPacket->clientIndex);

	//Cannot load
	if (accountIndex < 0) {
		//build the message
		std::ostringstream msg;
		msg << "Account already loaded: " << argPacket->username;

		//build the packet
		TextPacket newPacket;
		newPacket.type = SerialPacketType::LOGIN_REJECTION;
		strncpy(newPacket.text, msg.str().c_str(), PACKET_STRING_SIZE);
		network.SendTo(clientData->GetAddress(), static_cast<SerialPacket*>(&newPacket));

		//log the error
		std::cerr << "Error message sent: " << newPacket.text << std::endl;
		return;
	}

	//send the client their info
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::LOGIN_RESPONSE;
	newPacket.clientIndex = argPacket->clientIndex;
	newPacket.accountIndex = accountIndex;

	network.SendTo(clientData->GetAddress(), static_cast<SerialPacket*>(&newPacket));

	//finished this routine
	std::cout << "New login, " << clientMgr.GetLoadedCount() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

void ServerApplication::hLogoutRequest(ClientPacket* const argPacket) {
	//get the account and client data
	AccountData* accountData = accountMgr.Get(argPacket->accountIndex);
	if (!accountData) {
		return;
	}

	ClientData* clientData = clientMgr.Get(accountData->GetClientIndex());
	if (!clientData) {
		std::ostringstream msg;
		msg << "No client found for an account: " << accountData->GetUsername();
		throw(std::logic_error(msg.str()));
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified logout detected targeting: " << accountData->GetUsername() << std::endl;
		return;
	}

	//send the logout response
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::LOGOUT_RESPONSE;
	newPacket.accountIndex = argPacket->accountIndex;

	network.SendTo(clientData->GetAddress(), static_cast<SerialPacket*>(&newPacket));

	//save and unload this account and it's characters
	FullAccountUnload(argPacket->accountIndex);

	//finished this routine
	std::cout << "New logout, " << clientMgr.GetLoadedCount() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;
}

void ServerApplication::hDisconnectRequest(ClientPacket* const argPacket) {
	//get the client data
	ClientData* clientData = clientMgr.Get(argPacket->clientIndex);
	if (!clientData) {
		return;
	}

	//check for fraud
	if (clientData->GetAddress() != argPacket->srcAddress) {
		std::cerr << "Falsified disconnection detected targeting: " << argPacket->clientIndex << std::endl;
		return;
	}

	//send the disconnect response
	ClientPacket newPacket;
	newPacket.type = SerialPacketType::DISCONNECT_RESPONSE;
	newPacket.clientIndex = argPacket->clientIndex;

	network.SendTo(clientData->GetAddress(), static_cast<SerialPacket*>(&newPacket));

	//unload the client, it's accounts, and their characters
	FullClientUnload(argPacket->clientIndex);

	//finished this routine
	std::cout << "New disconnection, " << clientMgr.GetLoadedCount() << " clients and " << accountMgr.GetLoadedCount() << " accounts total" << std::endl;	
}
