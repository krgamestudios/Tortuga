--the indeces of tiles from an associated tileset
--this file is intended for use with a generator

--store the tilesheet's format
local format = {
	fname = "roguetileset.png",
	sheetWidth = 256,
	sheetHeight = 256,
	tileWidth = 16,
	tileHeight = 16,
	countX = 16,
	countY = 16
}

--define the walls of the simple "walls" set
local walls = {
	--concave
	nw = format.countX * 0 + 1,	n = format.countX * 0 + 2,	ne = format.countX * 0 + 3,
	w  = format.countX * 1 + 1,								e  = format.countX * 1 + 3,
	sw = format.countX * 2 + 1,	s = format.countX * 2 + 2,	se = format.countX * 2 + 3,
	--convex (corner pieces jutting out)
	--DOCS: These are named according to the corresponding tile they fit
	sec = format.countX * 0 + 7, swc = format.countX * 0 + 9,
	nec = format.countX * 2 + 7, nwc = format.countX * 2 + 9
}

--define the corridor pieces
--named for their door locations in the order nesw
local corridors = {
	--straight
	ns = 10, ew = 11,

	--dead ends
	s = format.countX * 0 + 8,
	w = format.countX * 1 + 9,
	n = format.countX * 2 + 8,
	e = format.countX * 1 + 7,

	--loop (compass)
	es  = format.countX * 0 + 4, esw  = format.countX * 0 + 5, sw  = format.countX * 0 + 6,
	nes = format.countX * 1 + 4, nesw = format.countX * 1 + 5, nsw = format.countX * 1 + 6,
	ne  = format.countX * 2 + 4, new  = format.countX * 2 + 5, nw  = format.countX * 2 + 6
}

--DEBUG: dump the values
--[[
print("walls:")
for k, v in pairs(walls) do
	print("", k, v)
end
print("corridors:")
for k, v in pairs(corridors) do
	print("", k, v)
end
--]]

--DEBUG: test the values
--[[
local test = {}

for k, v in pairs(walls) do
	if test[v] then
		print("Error found: ", k)
		return
	end
	test[v] = true
end

for k, v in pairs(corridors) do
	if test[v] then
		print("Error found: ", k)
		return
	end
	test[v] = true
end

print("No errors detected")
--]]

return {format, walls, corridors}