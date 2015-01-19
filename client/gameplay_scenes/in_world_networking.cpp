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
#include "in_world.hpp"

#include "channels.hpp"
#include "ip_operators.hpp"
#include "terminal_error.hpp"

#include <chrono>
#include <sstream>
#include <stdexcept>

//-------------------------
//Basic connections
//-------------------------

void InWorld::HandlePacket(SerialPacket* const argPacket) {
	switch(argPacket->type) {
		//heartbeat system
		case SerialPacketType::PING:
			HandlePing(static_cast<ServerPacket*>(argPacket));
		break;
		case SerialPacketType::PONG:
			HandlePong(static_cast<ServerPacket*>(argPacket));
		break;

		//game server connections
		case SerialPacketType::LOGOUT_RESPONSE:
			HandleLogoutResponse(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::DISCONNECT_RESPONSE:
			HandleDisconnectResponse(static_cast<ClientPacket*>(argPacket));
		break;
		case SerialPacketType::DISCONNECT_FORCED:
			HandleDisconnectForced(static_cast<ClientPacket*>(argPacket));
		break;

		//map management
		case SerialPacketType::REGION_CONTENT:
			HandleRegionContent(static_cast<RegionPacket*>(argPacket));
		break;

		//character management
		case SerialPacketType::CHARACTER_CREATE:
			HandleCharacterCreate(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_DELETE:
			HandleCharacterDelete(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_CHARACTER_EXISTS:
			HandleCharacterQueryExists(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_MOVEMENT:
			HandleCharacterMovement(static_cast<CharacterPacket*>(argPacket));
		break;
		case SerialPacketType::CHARACTER_ATTACK:
			HandleCharacterAttack(static_cast<CharacterPacket*>(argPacket));
		break;

		//monster management
		case SerialPacketType::MONSTER_CREATE:
			HandleMonsterCreate(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::MONSTER_DELETE:
			HandleMonsterDelete(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::QUERY_MONSTER_EXISTS:
			HandleMonsterQueryExists(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::MONSTER_MOVEMENT:
			HandleMonsterMovement(static_cast<MonsterPacket*>(argPacket));
		break;
		case SerialPacketType::MONSTER_ATTACK:
			HandleMonsterAttack(static_cast<MonsterPacket*>(argPacket));
		break;

		//rejection messages
		case SerialPacketType::REGION_REJECTION:
		case SerialPacketType::CHARACTER_REJECTION:
			throw(terminal_error(static_cast<TextPacket*>(argPacket)->text));
		break;
		case SerialPacketType::SHUTDOWN_REJECTION:
			throw(std::runtime_error(static_cast<TextPacket*>(argPacket)->text));
		break;

		//errors
		default: {
			std::ostringstream msg;
			msg << "Unknown SerialPacketType encountered in InWorld: " << static_cast<int>(argPacket->type);
			throw(std::runtime_error(msg.str()));
		}
		break;
	}
}

void InWorld::HandlePing(ServerPacket* const argPacket) {
	ServerPacket newPacket;
	newPacket.type = SerialPacketType::PONG;
	network.SendTo(argPacket->srcAddress, &newPacket);
}

void InWorld::HandlePong(ServerPacket* const argPacket) {
	if (*network.GetIPAddress(Channels::SERVER) != argPacket->srcAddress) {
		throw(std::runtime_error("Heartbeat message received from an unknown source"));
	}
	attemptedBeats = 0;
	lastBeat = Clock::now();
}

//-------------------------
//Connection control
//-------------------------

void InWorld::SendLogoutRequest() {
	ClientPacket newPacket;

	//send a logout request
	newPacket.type = SerialPacketType::LOGOUT_REQUEST;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::SendDisconnectRequest() {
	ClientPacket newPacket;

	//send a disconnect request
	newPacket.type = SerialPacketType::DISCONNECT_REQUEST;
	newPacket.clientIndex = clientIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::SendShutdownRequest() {
	ClientPacket newPacket;

	//send a shutdown request
	newPacket.type = SerialPacketType::SHUTDOWN_REQUEST;
	newPacket.accountIndex = accountIndex;

	network.SendTo(Channels::SERVER, &newPacket);
}

void InWorld::HandleLogoutResponse(ClientPacket* const argPacket) {
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

void InWorld::HandleDisconnectResponse(ClientPacket* const argPacket) {
	HandleLogoutResponse(argPacket);//shortcut
	SetNextScene(SceneList::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have successfully logged out";
}

void InWorld::HandleDisconnectForced(ClientPacket* const argPacket) {
	HandleDisconnectResponse(argPacket);//shortcut
	SetNextScene(SceneList::DISCONNECTEDSCREEN);
	ConfigUtility::GetSingleton()["client.disconnectMessage"] = "You have been forcibly disconnected by the server";
}

void InWorld::CheckHeartBeat() {
	//check the connection (heartbeat)
	if (Clock::now() - lastBeat > std::chrono::seconds(3)) {
		if (attemptedBeats > 2) {
			//escape to the disconnect screen
			SendDisconnectRequest();
			SetNextScene(SceneList::DISCONNECTEDSCREEN);
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
//map management
//-------------------------

void InWorld::SendRegionRequest(int roomIndex, int x, int y) {
	RegionPacket packet;

	//pack the region's data
	packet.type = SerialPacketType::REGION_REQUEST;
	packet.roomIndex = roomIndex;
	packet.x = x;
	packet.y = y;

	network.SendTo(Channels::SERVER, &packet);
}

void InWorld::HandleRegionContent(RegionPacket* const argPacket) {
	//replace existing regions
	regionPager.UnloadIf([&](Region const& region) -> bool {
		return region.GetX() == argPacket->x && region.GetY() == argPacket->y;
	});
	regionPager.PushRegion(argPacket->region);

	//clean up after the serial code
	delete argPacket->region;
	argPacket->region = nullptr;
}

void InWorld::UpdateMap() {
	if (roomIndex == -1) {
		return;
	}

	//these represent the zone of regions that the client needs loaded, including the mandatory buffers (+1/-1)
	int xStart = snapToBase(REGION_WIDTH, camera.x/tileSheet.GetTileW()) - REGION_WIDTH;
	int xEnd = snapToBase(REGION_WIDTH, (camera.x+camera.width)/tileSheet.GetTileW()) + REGION_WIDTH;

	int yStart = snapToBase(REGION_HEIGHT, camera.y/tileSheet.GetTileH()) - REGION_HEIGHT;
	int yEnd = snapToBase(REGION_HEIGHT, (camera.y+camera.height)/tileSheet.GetTileH()) + REGION_HEIGHT;

	//prune distant regions
	regionPager.GetContainer()->remove_if([&](Region const& region) -> bool {
		return region.GetX() < xStart || region.GetX() > xEnd || region.GetY() < yStart || region.GetY() > yEnd;
	});

	//request empty regions within this zone
	for (int i = xStart; i <= xEnd; i += REGION_WIDTH) {
		for (int j = yStart; j <= yEnd; j += REGION_HEIGHT) {
			if (!regionPager.FindRegion(i, j)) {
				SendRegionRequest(roomIndex, i, j);
			}
		}
	}
}