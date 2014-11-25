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

--create the overworld, set it's generator, loader & saver
--[[
local t = {
	"overworld.bmp", --tileset name
	mapSaver.load, --load function
	mapSaver.save, --save function
	mapMaker.debugIsland, --create function
	mapSaver.save --unload function
}]]

dumpTable(roomSystem)
dumpTable(roomSystem.RoomManager)
dumpTable(roomSystem.Room)

local overworld = roomSystem.RoomManager.CreateRoom("overworld")
roomSystem.Room.Initialize(overworld, "overworld.bmp", mapSaver.Load, mapSaver.Save, mapMaker.debugIsland, mapSaver.Save)

print("Finished the lua script")
