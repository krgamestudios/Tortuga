print("Lua script check")

--requirements
roomManagerAPI = require("room_manager")
roomAPI = require("room")

mapMaker = require("map_maker")
mapSaver = require("map_saver")

characterAPI = require("character")
entityAPI = require("entity")
networkAPI = require("network")

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
regionPagerAPI = require("region_pager")
triggerManagerAPI = require("trigger_manager")

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

--simple door pair
createTrigger("door 1", overworld, 128, -128, function(entity)
	if entityAPI.GetType(entity) ~= "character" then
		return
	end

	local x, y = characterAPI.GetOrigin(entity)
	characterAPI.SetRoomIndex(entity, uidTwo) --TODO: (1) take exit coordinates as a parameter
	characterAPI.SetOrigin(entity, 0, 0)
	networkAPI.PumpCharacterUpdate(entity)
end)

createTrigger("door 1", underworld, 128, -128, function(entity)
	if entityAPI.GetType(entity) ~= "character" then
		return
	end

	local x, y = characterAPI.GetOrigin(entity)
	characterAPI.SetRoomIndex(entity, uidOne)
	characterAPI.SetOrigin(entity, 0, 0)
	networkAPI.PumpCharacterUpdate(entity)
end)

print("Finished the lua script")
