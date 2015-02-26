local Region = require("map_system").Region

local mapMaker = {}

--utility functions
function mapMaker.Sqr(x) return x*x end
function mapMaker.Dist(x, y, i, j) return math.sqrt(mapMaker.Sqr(x - i) + mapMaker.Sqr(y - j)) end

--tile macros, mapped to the tilesheet "overworld.bmp"
mapMaker.blank	= 0
mapMaker.water	= 18 + 3 * 0
mapMaker.sand	= 18 + 3 * 1
mapMaker.plains	= 18 + 3 * 2
mapMaker.grass	= 18 + 3 * 3
mapMaker.dirt	= 18 + 3 * 4

--"edge" macros
mapMaker.edges = {}
mapMaker.edges.north = -16
mapMaker.edges.south = 16
mapMaker.edges.east = 1
mapMaker.edges.west = -1

--use these macros (mapped to "overworld.bmp" for now) to smooth the region's edges
function mapMaker.SmoothEdgesSimple(r)
	--make and pad an array to use
	local shiftArray = {}
	for i = 1, Region.GetWidth(r) do
		shiftArray[i] = {}
		for j = 1, Region.GetHeight(r) do
			shiftArray[i][j] = 0
		end
	end

	--build the array
	for i = 1, Region.GetWidth(r) do
		for j = 1, Region.GetHeight(r) do
			--if (not region edge) and (west tile < this tile), etc.
			if i > 1 and Region.GetTile(r, i - 1, j, 1) < Region.GetTile(r, i, j, 1) then
				shiftArray[i][j] = shiftArray[i][j] + mapMaker.edges.west
			end
			if j > 1 and Region.GetTile(r, i, j - 1, 1) < Region.GetTile(r, i, j, 1) then
				shiftArray[i][j] = shiftArray[i][j] + mapMaker.edges.north
			end
			if i < Region.GetWidth(r) and Region.GetTile(r, i + 1, j, 1) < Region.GetTile(r, i, j, 1) then
				shiftArray[i][j] = shiftArray[i][j] + mapMaker.edges.east
			end
			if j < Region.GetHeight(r) and Region.GetTile(r, i, j + 1, 1) < Region.GetTile(r, i, j, 1) then
				shiftArray[i][j] = shiftArray[i][j] + mapMaker.edges.south
			end
		end
	end

	--finally apply this
	for i = 1, Region.GetWidth(r) do
		for j = 1, Region.GetHeight(r) do
			if shiftArray[i][j] ~= 0 then
				Region.SetTile(r, i, j, 2, Region.GetTile(r, i, j, 1) + shiftArray[i][j])
				Region.SetTile(r, i, j, 1, Region.GetTile(r, i, j, 1) - 3)
			end
		end
	end
end

function mapMaker.PlaceMonsterSpawn(r, x, y, script)
	--place monster spawns here, highlighted by dirt patches

	--wrong region
	if  x < Region.GetX(r) or x >= Region.GetX(r) + Region.GetWidth(r) or
		y < Region.GetY(r) or y >= Region.GetY(r) + Region.GetHeight(r)
		then
		return
	end

	--place a dirt tile, clearing the above layers
	Region.SetTile(r, x - Region.GetX(r), y - Region.GetY(r), 1, mapMaker.dirt)
	Region.SetTile(r, x - Region.GetX(r), y - Region.GetY(r), 2, mapMaker.blank)
	Region.SetTile(r, x - Region.GetX(r), y - Region.GetY(r), 3, mapMaker.blank)

	--TODO: (1) create a monster spawn trigger using the given script
end

--custom generation systems here
function mapMaker.DebugIsland(r)
	--basic distance check for each tile, placing an island around the world origin
	for i = 1, Region.GetWidth(r) do
		for j = 1, Region.GetHeight(r) do
			local dist = mapMaker.Dist(0, 0, i + Region.GetX(r) -1, j + Region.GetY(r) -1)
			if dist < 10 then
				Region.SetTile(r, i, j, 1, mapMaker.plains)
			elseif dist < 12 then
				Region.SetTile(r, i, j, 1, mapMaker.sand)
			else
				Region.SetTile(r, i, j, 1, mapMaker.water)
				Region.SetSolid(r, i, j, true)
			end
		end
	end

	--examples of the smoothing function NOT working correctly
	--[[
	for j = 1, Region.GetHeight(r) do
		Region.SetTile(r, 3, j, 1, mapMaker.dirt)
		Region.SetTile(r, 4, j, 1, mapMaker.dirt)

		Region.SetTile(r, 10, j, 1, mapMaker.dirt)
	end
	--]]

	--A generic edge system
	mapMaker.SmoothEdgesSimple(r)

	--place monster spawns
	mapMaker.PlaceMonsterSpawn(r, -5, -5, nil)
end

return mapMaker