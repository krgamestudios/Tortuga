--TODO: each function needs to check it's parameter count
--TODO: Set metatables
--TODO: test

--RoomManager interface
RoomMgr.GetRoom(index)
RoomMgr.CreateRoom([{params}])
RoomMgr.UnloadRoom(index)


--RoomData interface
Room.GetPager()
Room.GetGenerator()
Room.OnCreate([{params}])
Room.OnUnload()

Room.Get{params}()


--RegionPager interface
RegionPager.SetTile(x, y, z, v)
RegionPager.GetTile(x, y, z)
RegionPager.GetRegion(x, y)
RegionPager.SetDirectory(directory)
RegionPager.GetDirectory()
RegionPager.LoadRegion(x, y)
RegionPager.SaveRegion(x, y)
RegionPager.CreateRegion(x, y[, {params}])
RegionPager.UnloadRegion(x, y)


--Region interface
Region.SetTile(x, y, z, v)
Region.GetTile(x, y, z)
Region.GetX()
Region.GetY()
Region.GetWidth()
Region.GetHeight()
Region.GetDepth()
Region.OnLoad(directory)
Region.OnSave(directory)
Region.OnCreate([{params}])
Region.OnUnload([directory])


--*Generator interface
Generator.GetMapType()
Generator.GetChunk(x, y)
Generator.GetMapWidth()
Generator.GetMapHeight()


--Chunk interface
??