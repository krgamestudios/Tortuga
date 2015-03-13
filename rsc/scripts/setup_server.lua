print("Lua script check")

--requirements
roomManagerAPI = require("room_manager")
roomAPI = require("room")

mapMaker = require("map_maker")
mapSaver = require("map_saver")

characterAPI = require("character")
entityAPI = require("entity")
networkAPI = require("network")

regionPagerAPI = require("region_pager")
triggerAPI = require("trigger")
triggerManagerAPI = require("trigger_manager")

--test the room hooks
roomManagerAPI.SetOnCreate(function(room, index)
	print("", "Creating room: ", roomAPI.GetName(room), index)

	roomAPI.SetOnTick(room, function(room)
		roomAPI.ForEachCharacter(room, function(character)
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

--NOTE: test the trigger system

function createTrigger(handle, room, x, y, script)
	local pager = roomAPI.GetPager(room)

	--place the indicator tile
	regionPagerAPI.SetTile(pager, x / 32, y / 32, 0, mapMaker.dirt)
	regionPagerAPI.SetTile(pager, x / 32, y / 32, 1, mapMaker.blank)
	regionPagerAPI.SetTile(pager, x / 32, y / 32, 2, mapMaker.blank)

	--create the trigger object
	triggerManagerAPI.Create(
		roomAPI.GetTriggerMgr(room), handle, x, y,
		0, 0, 32, 32, --size of the tiles
		script
		)
end

function createDoorPair(handle, roomOne, roomOneUID, Xone, Yone, roomTwo, roomTwoUID, Xtwo, Ytwo)
	--create the scripts
	local function scriptOne(entity)
		if entityAPI.GetType(entity) ~= "character" then return end

		--move the character
		characterAPI.SetRoomIndex(entity, roomTwoUID)
		characterAPI.SetOrigin(entity, Xtwo, Ytwo-16)
		networkAPI.PumpCharacterUpdate(entity)

		--disable the other trigger
		local triggerTwo = triggerManagerAPI.GetTrigger(roomAPI.GetTriggerMgr(roomTwo), handle)
		triggerAPI.PushExclusionEntity(triggerTwo, entity)
	end

	local function scriptTwo(entity)
		if entityAPI.GetType(entity) ~= "character" then return end

		--move the character
		characterAPI.SetRoomIndex(entity, roomOneUID)
		characterAPI.SetOrigin(entity, Xone, Yone-16) --NOTE: the 16 pixel margin for presentation
		networkAPI.PumpCharacterUpdate(entity)

		--disable the other trigger
		local triggerOne = triggerManagerAPI.GetTrigger(roomAPI.GetTriggerMgr(roomOne), handle)
		triggerAPI.PushExclusionEntity(triggerOne, entity)
	end

	--create the triggers proper
	createTrigger(handle, roomOne, Xone, Yone, scriptOne)
	createTrigger(handle, roomTwo, Xtwo, Ytwo, scriptTwo)
end

--call the monstrosity
createDoorPair("pair 1", overworld, uidOne, 0, -64, underworld, uidTwo, 0, 0)
createDoorPair("pair 2", overworld, uidOne, 64, -64, underworld, uidTwo, 64, 0)
createDoorPair("pair 3", overworld, uidOne, 128, -64, underworld, uidTwo, 128, 0)

--[[
--simple door pair
createTrigger("door 1", overworld, 128, -128, function(entity)
	if entityAPI.GetType(entity) ~= "character" then
		return
	end

	--move the character
	characterAPI.SetRoomIndex(entity, uidTwo)
	characterAPI.SetOrigin(entity, 0, 0)
	networkAPI.PumpCharacterUpdate(entity)
end)

createTrigger("door 1", underworld, 128, -128, function(entity)
	if entityAPI.GetType(entity) ~= "character" then
		return
	end

	--move the character
	characterAPI.SetRoomIndex(entity, uidOne)
	characterAPI.SetOrigin(entity, 0, 0)
	networkAPI.PumpCharacterUpdate(entity)
end)
--]]

print("Finished the lua script")
