print("Lua script check")

mapSystem = require "map_system"
mapMaker = require "map_maker"
mapSaver = require "map_saver"
roomSystem = require "room_system"
waypointSystem = require "waypoint_system"

local function dumpTable(t)
	print(t)
	for k, v in pairs(t) do
		print("",k,v)
	end
end

--NOTE: room 0 is the first that the client asks for, therefore it must exist
local overworld, uid = roomSystem.RoomManager.CreateRoom("overworld", "overworld.bmp")

--NOTE: This is horrible; room initialization is important
mapSystem.RegionPager.SetOnLoad(roomSystem.Room.GetPager(overworld), mapSaver.Load)
mapSystem.RegionPager.SetOnSave(roomSystem.Room.GetPager(overworld), mapSaver.Save)
mapSystem.RegionPager.SetOnCreate(roomSystem.Room.GetPager(overworld), mapMaker.debugIsland)
mapSystem.RegionPager.SetOnUnload(roomSystem.Room.GetPager(overworld), mapSaver.Save)

print("Finished the lua script")
