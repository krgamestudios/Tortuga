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

entityAPI = require("entity")

for k, v in pairs(entityAPI) do
	print(k, v)
end

--requirements
roomManagerAPI = require("room_manager")
roomAPI = require("room")

mapMaker = require("map_maker")
mapSaver = require("map_saver")

doorUtility = require("door_utility")

creatureAPI = require("creature")
creatureManagerAPI = require("creature_manager")
barrierAPI = require("barrier")
barrierManagerAPI = require("barrier_manager")

--testing creature tags
local function bunnySquare(creature)
	--return value
	local ret = 0

	--these tags are strings
	local direction = creatureAPI.GetTag(creature, "direction")
	local timestamp = creatureAPI.GetTag(creature, "timestamp")

	--has this creature been set up yet?
	if string.len(direction) == 0 then
		direction = "south"
		timestamp = tostring(os.time())
		creatureAPI.SetMotion(creature, 0, 1)
	end

	--is it time to change direction?
	if os.time() - tonumber(timestamp) >= 4 then
		if string.match("south", direction) then
			direction = "east"
			creatureAPI.SetMotion(creature, 1, 0)
		else
		if string.match("east", direction) then
			direction = "north"
			creatureAPI.SetMotion(creature, 0, -1)
		else
		if string.match("north", direction) then
			direction = "west"
			creatureAPI.SetMotion(creature, -1, 0)
		else --south is a default
			direction = "south"
			creatureAPI.SetMotion(creature, 0, 1)
		end end end
		timestamp = tostring(os.time())
		ret = 1
	end

	creatureAPI.SetTag(creature, "direction", direction)
	creatureAPI.SetTag(creature, "timestamp", timestamp)
	return ret
end

local function barrierTick(barrier)
	local timestamp = barrierAPI.GetTag(barrier, "timestamp")

	--initialize the timestamp
	if string.len(timestamp) == 0 then
		timestamp = tostring(os.time())
		barrierAPI.SetTag(barrier, "timestamp", timestamp)
	end

	--is it time to change the display?
	if os.time() - tonumber(timestamp) < 1 then
		return 0
	else
		timestamp = tostring(os.time())
		barrierAPI.SetTag(barrier, "timestamp", timestamp)
	end

	--binary tick
	for i = 1, 8 do
		if barrierAPI.GetStatus(barrier, i) == 0 then
			barrierAPI.SetStatus(barrier, i, 1)
			return 1
		else
			barrierAPI.SetStatus(barrier, i, 0)
		end
	end
	return 0
end

--test the room hooks
roomManagerAPI.SetOnCreate(function(room, index)
	print("", "Creating room: ", roomAPI.GetName(room), index)

	--TODO:creatureManager with SetOnCreate, SetOnUnload & create
	creatureManagerAPI.SetOnCreate(roomAPI.GetCreatureMgr(room), function(creature, index)
		--
	end)

	creatureManagerAPI.SetOnUnload(roomAPI.GetCreatureMgr(room), function(creature)
		--
	end)

	creatureManagerAPI.Create(roomAPI.GetCreatureMgr(room), "anibunny.png", bunnySquare)

	--TODO: (0) barrierManager with Create
	barrierManagerAPI.SetOnCreate(roomAPI.GetBarrierMgr(room), function(barrier)
		barrierAPI.SetScript(barrier, barrierTick)
	end)

	roomAPI.SetOnTick(room, function(room)
		roomAPI.ForEachCharacter(room, function(character)
			--
		end)

		roomAPI.ForEachCreature(room, function(creature)
			--
		end)

		--TODO: for each barrier
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

