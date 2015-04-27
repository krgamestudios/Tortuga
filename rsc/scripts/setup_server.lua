print("Lua script check")

--requirements
roomManagerAPI = require("room_manager")
roomAPI = require("room")

mapMaker = require("map_maker")
mapSaver = require("map_saver")

doorUtility = require("door_utility")

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

--call the monstrosity
doorUtility.createDoorPair("pair 1", overworld, -64, -64, underworld, 64, 64)

print("Finished the lua script")
