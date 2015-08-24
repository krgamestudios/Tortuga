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
#include "world.hpp"

#include "channels.hpp"
#include "ip_operators.hpp"

#include <chrono>
#include <sstream>
#include <stdexcept>

//-------------------------
//heartbeat system
//-------------------------

void World::hPing(ServerPacket* const argPacket) {
	ServerPacket newPacket;
	newPacket.type = SerialPacketType::PONG;
	network.SendTo(argPacket->srcAddress, &newPacket);
}

void World::hPong(ServerPacket* const argPacket) {
	if (*network.GetIPAddress(Channels::SERVER) != argPacket->srcAddress) {
		throw(std::runtime_error("Heartbeat message received from an unknown source"));
	}
	attemptedBeats = 0;
	lastBeat = Clock::now();
}

void World::CheckHeartBeat() {
	//check the connection (heartbeat)
	if (Clock::now() - lastBeat > std::chrono::seconds(3)) {
		if (attemptedBeats > 2) {
			//escape to the disconnect screen
			SendDisconnectRequest();
			SetSceneSignal(SceneSignal::DISCONNECTEDSCREEN);
			ConfigUtility::GetSingleton()["client.disconnectMessage"] = "Error: Lost connection to the server";
		}
		else {
			ServerPacket newPacket;
			newPacket.type = SerialPacketType::PING;
			network.SendTo(Channels::SERVER, &newPacket);

			attemptedBeats++;
			lastBeat = Clock::now();
		}
	}
}

//-------------------------
//Connection control
//-------------------------

void World::SendLogoutRequest() {
	ClientPacket newPacket;

	//send a logout request
	newPacket.type = SerialPacketType::LOGOUT_REQUEST;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void World::SendDisconnectRequest() {
	ClientPacket newPacket;

	//send a disconnect request
	newPacket.type = SerialPacketType::DISCONNECT_REQUEST;
	newPacket.clientIndex = clientIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void World::SendAdminDisconnectForced() {
	//TODO: (9) World::SendAdminDisconnectForced()
}

void World::SendAdminShutdownRequest() {
	ClientPacket newPacket;

	//send a shutdown request
	newPacket.type = SerialPacketType::ADMIN_SHUTDOWN_REQUEST;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void World::hLogoutResponse(ClientPacket* const argPacket) {
	if (localCharacter) {
		characterMap.erase(characterIndex);
		localCharacter = nullptr;
	}

	accountIndex = -1;
	characterIndex = -1;

	//reset the camera
	camera.marginX = camera.marginY = 0;

	//because, why not? I guess...
	SendDisconnectRequest();
}

void World::hDisconnectResponse(ClientPacket* const argPacket) {
	hLogoutResponse(argPacket);//shortcut
	SetSceneSignal(SceneSignal::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have successfully logged out";
}

void World::hAdminDisconnectForced(ClientPacket* const argPacket) {
	hDisconnectResponse(argPacket);//shortcut
	SetSceneSignal(SceneSignal::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have been forcibly disconnected by the server";
}

