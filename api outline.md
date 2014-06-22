--RoomManager interface
roommgr.getroom(index)
	return userdata
roommgr.createroom(room[, {params}])
	return userdata
roommgr.unloadroom(room)
	return nil


--RoomData interface
room.getpager(room)
	return userdata
room.getgenerator(room)
	return userdata

room.get{params}(room)


--RegionPager interface
pager.settile(p, x, y, z, v)
pager.gettile(p, x, y, z)
pager.getregion(p, x, y)
pager.setdirectory(p, directory)
pager.getdirectory(p)
pager.loadregion(p, x, y)
pager.saveregion(p, x, y)
pager.createregion(p, x, y[, {params}])
pager.unloadregion(p, x, y)


--Region interface
region.settile(r, x, y, z, v)
region.gettile(r, x, y, z)
region.getx(r)
region.gety(r)
region.getwidth()
region.getheight()
region.getdepth()
region.load(r, directory)
region.save(r, directory)
region.create(r[, {params}])
region.unload(r[, directory])


--*Generator interface
generator.gettype(generator)
	return string
generator.getchunk(generator, x, y)
	return userdata
generator.getmapwidth()
	return number
generator.getmapheight()
	return number


--Chunk interface
??