print("Lua script check")

mapMaker = require "map_maker"
mapSaver = require "map_saver"
roomSystem = require "room_system"

--NOTE: room 0 is the first that the client asks for, therefore it must exist
local overworld, uid = roomSystem.RoomManager.CreateRoom("overworld")
roomSystem.Room.Initialize(overworld, "overworld.bmp", mapSaver.Load, mapSaver.Save, mapMaker.debugIsland, mapSaver.Save)

print("Finished the lua script")
