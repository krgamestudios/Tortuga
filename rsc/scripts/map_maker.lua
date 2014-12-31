local mapSystem = require "map_system"

local mapMaker = {}

--utility functions
function mapMaker.sqr(x) return x*x end
function mapMaker.dist(x, y, i, j) return math.sqrt(mapMaker.sqr(x - i) + mapMaker.sqr(y - j)) end

--tile macros, mapped to the tilesheet "overworld.bmp"
mapMaker.edges = {}
mapMaker.edges.north = -16
mapMaker.edges.south = 16
mapMaker.edges.east = 1
mapMaker.edges.west = -1

mapMaker.water	= 18 + 3 * 0
mapMaker.sand	= 18 + 3 * 1
mapMaker.plains	= 18 + 3 * 2
mapMaker.grass	= 18 + 3 * 3
mapMaker.dirt	= 18 + 3 * 4

--custom generation systems here
function mapMaker.debugIsland(region)
	for i = 1, mapSystem.Region.GetWidth(region) do
		for j = 1, mapSystem.Region.GetHeight(region) do
			local dist = mapMaker.dist(0, 0, i + mapSystem.Region.GetX(region) -1, j + mapSystem.Region.GetY(region) -1)
			if dist < 10 then
				mapSystem.Region.SetTile(region, i, j, 1, mapMaker.plains)
			elseif dist < 12 then
				mapSystem.Region.SetTile(region, i, j, 1, mapMaker.sand)
			else
				mapSystem.Region.SetTile(region, i, j, 1, mapMaker.water)
				mapSystem.Region.SetSolid(region, i, j, true)
			end
		end
	end
end

function mapMaker.dirtLand(region)
	for i = 1, mapSystem.Region.GetWidth(region) do
		for j = 1, mapSystem.Region.GetHeight(region) do
			mapSystem.Region.SetTile(region, i, j, 1, mapMaker.dirt)
		end
	end
end

return mapMaker