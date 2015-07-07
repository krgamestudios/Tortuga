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
#include "network_api.hpp"

#include "character_data.hpp"
#include "character_manager.hpp"
#include "server_utilities.hpp"

#include <cstring>

static int pumpCharacterUpdate(lua_State* L) {
	CharacterData* characterData = static_cast<CharacterData*>(lua_touserdata(L, 1));

	//determine the character's index
	int index = -1;
	for (auto const& it : *CharacterManager::GetSingleton().GetContainer()) {
		if(characterData == &it.second) {
			index = it.first;
			break;
		}
	}

	//signal an error
	if (index == -1) {
		lua_pushboolean(L, false);
		return 1;
	}

	//fill the packet with all of this character's data
	CharacterPacket newPacket;
	newPacket.type = SerialPacketType::CHARACTER_UPDATE;
	newPacket.characterIndex = index;
	strncpy(newPacket.handle, characterData->GetHandle().c_str(), PACKET_STRING_SIZE);
	strncpy(newPacket.avatar, characterData->GetAvatar().c_str(), PACKET_STRING_SIZE);
	newPacket.accountIndex = characterData->GetOwner();
	newPacket.roomIndex = characterData->GetRoomIndex();
	newPacket.origin = characterData->GetOrigin();
	newPacket.motion = characterData->GetMotion();

	//pump to the room
	pumpPacketProximity(&newPacket, characterData->GetRoomIndex());

	//signal success
	lua_pushboolean(L, true);
	return 1;
}

static int pumpMonsterUpdate(lua_State* L) {
	//TODO: (0) send the info about a specific monster instance
}

static const luaL_Reg networkLib[] = {
	{"PumpCharacterUpdate", pumpCharacterUpdate},
	{"PumpMonsterUpdate", pumpMonsterUpdate},
	{nullptr, nullptr}
};

LUAMOD_API int openNetworkAPI(lua_State* L) {
	luaL_newlib(L, networkLib);
	return 1;
}