print("Lua script check")

mapMaker = require "map_maker"
mapSaver = require "map_saver"
roomSystem = require "room_system"

--create the overworld, set it's generator, loader & saver
local overworld = roomSystem.CreateRoom("overworld", "overworld.bmp")
roomSystem.SetOnLoad(overworld, mapMaker.debugIsland, mapSaver.load, mapSaver.save)

print("Finished the lua script")
