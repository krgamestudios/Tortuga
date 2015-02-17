print("Lua script check")

mapMaker = require("map_maker")
mapSaver = require("map_saver")
roomSystem = require("room_system")

local function dumpTable(t)
	print(t)
	for k, v in pairs(t) do
		print("",k,v)
	end
end

--test the room hooks
roomSystem.RoomManager.SetOnCreate(function(room, index)
	print("", "Creating room: ", roomSystem.Room.GetName(room), index)
end)
roomSystem.RoomManager.SetOnUnload(function(room, index)
	print("", "Unloading room: ", roomSystem.Room.GetName(room), index)
end)

--NOTE: room 0 is the first that the client asks for, therefore it must exist
local overworld, uid = roomSystem.RoomManager.CreateRoom("overworld", "overworld.bmp")
roomSystem.Room.Initialize(overworld, mapSaver.Load, mapSaver.Save, mapMaker.DebugIsland, mapSaver.Save)

print("Finished the lua script")
