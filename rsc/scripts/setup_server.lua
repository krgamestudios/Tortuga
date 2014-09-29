print("Lua script check")

mapMaker = require "map_maker"

--BUG: #35 The server fails without at least one room
--TODO: Create rooms with names?
newRoom = RoomManager.CreateRoom()
pager = Room.GetPager(newRoom)
RegionPager.SetOnCreate(pager, mapMaker.debugIsland)

print("Finished the lua script")
