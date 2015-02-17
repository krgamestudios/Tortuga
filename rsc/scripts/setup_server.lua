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

--debugging (only works correctly with one room)
globalTickTest = {
	ticks = 0,
	start = 0
}

--test the room hooks
roomSystem.RoomManager.SetOnCreate(function(room, index)
	print("", "Creating room: ", roomSystem.Room.GetName(room), index)

	globalTickTest.start = os.clock()

	--called ~60 times per second
	roomSystem.Room.SetOnTick(room, function(room)
		globalTickTest.ticks = globalTickTest.ticks + 1
	end)
end)
roomSystem.RoomManager.SetOnUnload(function(room, index)
	print("", "Unloading room: ", roomSystem.Room.GetName(room), index)
	print("Time: ", (os.clock() - globalTickTest.start), "Ticks: ", globalTickTest.ticks)
end)

--NOTE: room 0 is the first that the client asks for, therefore it must exist
local overworld, uid = roomSystem.RoomManager.CreateRoom("overworld", "overworld.bmp")
roomSystem.Room.Initialize(overworld, mapSaver.Load, mapSaver.Save, mapMaker.DebugIsland, mapSaver.Save)

print("Finished the lua script")
