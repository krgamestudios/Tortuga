print("Lua script check")

mapMaker = require "map_maker"
mapSaver = require "map_saver"
roomSystem = require "room_system"

local function dumpTable(t)
	print(t)
	for k, v in pairs(t) do
		print("",k, v)
	end
end

dumpTable(mapMaker)
dumpTable(mapSaver)
dumpTable(roomSystem)
dumpTable(roomSystem.Room)
dumpTable(roomSystem.RoomManager)

--create the overworld, set it's generator, loader & saver
local overworld = roomSystem.RoomManager.CreateRoom("overworld", "overworld.bmp")
roomSystem.Room.SetOnLoad(overworld, 1123)
roomSystem.Room.SetOnUnload(overworld, 458)

if roomSystem.Room.GetOnLoad(overworld) == 1123 then
	print("onload retreival works")
end

if roomSystem.Room.GetOnUnload(overworld) == 458 then
	print("onunload retreival works")
end

print("Finished the lua script")
