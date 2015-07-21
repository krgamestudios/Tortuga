--generate a roguelike map

--[[ DOCS: Game design theory, read more

As a basic starting point, I can create a list of "seed" tiles, which are the
center of rooms. The rooms can be any size, as long as they don't overlap. From
There, a tunnel can be "carved" from each seed tile to the next. Finally, the 
map is populated, but that is beyond this exercise.

This specification isn't ironclad.

--]]

print("Beginning generator...")

function newroom(northBound, eastBound, southBound, westBound)
	--place the room within proximity of the center, so that they're not too far apart.
	local x = math.random(westBound, eastBound)
	local y = math.random(northBound, southBound)
	local w = math.random(2, 5)
	local h = math.random(2, 5)

	--return this new room
	return {
		seedX = x,
		seedY = y,
		--give it a decent size
		north = y - h,
		south = y + h,
		east = x - w,
		west = x + w,
	}
end

function newpath(x1, y1, x2, y2)
	--NOTE: a path is an ordered list of {x, y} pairs
	local path = {}
	local step = 0

	--vertical
	if y1 > y2 then
		step = -1
	else
		step = 1
	end
	for i = y1, y2, step do
		table.insert(path, {x = x1, y = i})
	end

	--horizontal
	if x1 > x2 then
		step = -1
	else
		step = 1
	end
	for i = x1, x2, step do
		table.insert(path, {x = i, y = y2})
	end
	
	--NOTE: {x, y} pairs are duplicated at the corners
	--TODO: improve the pathing system
	return path
end

function buildpaths(roomlist)
	local roomcount = #roomlist
	local pathlist = {}
	--tunnel the shortest paths
	for i = 1, roomcount-1 do
		table.insert(pathlist, newpath(
			roomlist[i].seedX,
			roomlist[i].seedY,
			roomlist[i+1].seedX,
			roomlist[i+1].seedY))
	end

	--return the new paths
	return pathlist
end

print("populating lists")

math.randomseed(os.time())

roomlist = {}
pathlist = {}

--populate the roomlist
roomcount = math.random(10, 15)
for i = 1, roomcount do
	table.insert(roomlist, newroom(-30, 0, 30, -60)) --60x60
end

roomcount = math.random(10, 15)
for i = 1, roomcount do
	table.insert(roomlist, newroom(-30, 50, 30, -10)) --60x60
end

roomcount = math.random(20, 30)
for i = 1, roomcount do
	table.insert(roomlist, newroom(-60, 120, 60, 70)) --50x120
end

pathlist = buildpaths(roomlist)

--pass the data onto the pager
pager = ... --called as a chunk

--create the rooms
for k, iter in next, roomlist do
	--for each tile in the room
	for i = iter.east, iter.west do
		for j = iter.north, iter.south do
			--set
			region_pager.SetTile(pager, i, j, 0, 14)
		end
	end
end

--create the paths
iter = nil
for k, path in next, pathlist do --multiple paths in the lsit
	for k, iter in next, path do
		--for each tile in the path, set
		region_pager.SetTile(pager, iter.x, iter.y, 0, 14)
	end
end

print("generator finished")