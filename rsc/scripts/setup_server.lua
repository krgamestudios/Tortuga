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

--[[
debugging test

Ideal output:

-------------------------
pager:	userdata:	[memory location]
Size 0:	0
[debug output from load]
Size 1: 1
[debug output from save]
Size 2: 0
[debug output from load]
Size 3: 1
[debug output from save]
Size 4: 0
-------------------------

--]-]

print("-------------------------")
local pager = roomSystem.Room.GetPager(overworld)

print("pager:", pager)

print("Size 0:", mapSystem.RegionPager.ContainerSize(pager))

local regionFoo = mapSystem.RegionPager.GetRegion(pager, 0, 0)
print("Size 1:", mapSystem.RegionPager.ContainerSize(pager))

mapSystem.RegionPager.UnloadRegion(pager, regionFoo)
print("Size 2:", mapSystem.RegionPager.ContainerSize(pager))

local regionFoo = mapSystem.RegionPager.GetRegion(pager, 0, 0)
print("Size 3:", mapSystem.RegionPager.ContainerSize(pager))

mapSystem.RegionPager.UnloadRegion(pager, 0, 0)
print("Size 4:", mapSystem.RegionPager.ContainerSize(pager))

print("-------------------------")
--]]