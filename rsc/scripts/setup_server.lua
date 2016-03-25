--[[
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
--]]

--DOCS: This script is run by the server on startup

print("Lua script check")

--requirements
roomManagerAPI = require("room_manager")
roomAPI = require("room")

mapMaker = require("map_maker")
mapSaver = require("map_saver")

doorUtility = require("door_utility")

creatureAPI = require("creature")
creatureManagerAPI = require("creature_manager")

--test the room hooks
roomManagerAPI.SetOnCreate(function(room, index)
	print("", "Creating room: ", roomAPI.GetName(room), index)

	creatureManagerAPI.Create(roomAPI.GetCreatureMgr(room), "anibunny.png", function() --[[]] end)

	roomAPI.SetOnTick(room, function(room)
		roomAPI.ForEachCharacter(room, function(character)
			--
		end)

		roomAPI.ForEachCreature(room, function(creature)
			--
		end)
	end)
end)

roomManagerAPI.SetOnUnload(function(room, index)
	print("", "Unloading room: ", roomAPI.GetName(room), index)
end)

--NOTE: room 0 is the first that the client asks for, therefore it must exist
local overworld, uidOne = roomManagerAPI.CreateRoom("overworld", "overworld.bmp")
roomAPI.Initialize(overworld, mapSaver.Load, mapSaver.Save, mapMaker.DebugIsland, mapSaver.Save)

local underworld, uidTwo = roomManagerAPI.CreateRoom("underworld", "overworld.bmp")
roomAPI.Initialize(underworld, mapSaver.Load, mapSaver.Save, mapMaker.DebugGrassland, mapSaver.Save)

--call the monstrosity
doorUtility.CreateDoorPair("pair 1", overworld, -64, -64, underworld, 64, 64)
