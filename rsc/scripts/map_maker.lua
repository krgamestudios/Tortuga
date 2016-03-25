--[[
/* Copyright: (c) Kayne Ruse 2013-2016
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
--]]

--DOCS: a placeholder map generator

local regionAPI = require("region")

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

--TODO: (1) path system
--use these macros (mapped to "overworld.bmp" for now) to smooth the region's edges
function mapMaker.SmoothEdgesSimple(r)
	--make and pad an array to use
	local shiftArray = {}
	for i = 1, regionAPI.GetWidth(r) do
		shiftArray[i] = {}
		for j = 1, regionAPI.GetHeight(r) do
			shiftArray[i][j] = 0
		end
	end

	--build the array
	for i = 1, regionAPI.GetWidth(r) do
		for j = 1, regionAPI.GetHeight(r) do
			--if (not regionAPI edge) and (west tile < this tile), etc.
			if i > 1 and regionAPI.GetTile(r, i - 1, j, 1) < regionAPI.GetTile(r, i, j, 1) then
				shiftArray[i][j] = shiftArray[i][j] + mapMaker.edges.west
			end
			if j > 1 and regionAPI.GetTile(r, i, j - 1, 1) < regionAPI.GetTile(r, i, j, 1) then
				shiftArray[i][j] = shiftArray[i][j] + mapMaker.edges.north
			end
			if i < regionAPI.GetWidth(r) and regionAPI.GetTile(r, i + 1, j, 1) < regionAPI.GetTile(r, i, j, 1) then
				shiftArray[i][j] = shiftArray[i][j] + mapMaker.edges.east
			end
			if j < regionAPI.GetHeight(r) and regionAPI.GetTile(r, i, j + 1, 1) < regionAPI.GetTile(r, i, j, 1) then
				shiftArray[i][j] = shiftArray[i][j] + mapMaker.edges.south
			end
		end
	end

	--finally apply this
	for i = 1, regionAPI.GetWidth(r) do
		for j = 1, regionAPI.GetHeight(r) do
			if shiftArray[i][j] ~= 0 then
				regionAPI.SetTile(r, i, j, 2, regionAPI.GetTile(r, i, j, 1) + shiftArray[i][j])
				regionAPI.SetTile(r, i, j, 1, regionAPI.GetTile(r, i, j, 1) - 3)
			end
		end
	end
end

--custom generation systems here
function mapMaker.DebugIsland(r)
	--debug
	io.write("map_maker:DebugIsland(", regionAPI.GetX(r), ", ", regionAPI.GetY(r), ")\n")

	--basic distance check for each tile, placing an island around the world origin
	for i = 1, regionAPI.GetWidth(r) do
		for j = 1, regionAPI.GetHeight(r) do
			local dist = mapMaker.Dist(0, 0, i + regionAPI.GetX(r) -1, j + regionAPI.GetY(r) -1)
			if dist < 10 then
				regionAPI.SetTile(r, i, j, 1, mapMaker.plains)
			elseif dist < 12 then
				regionAPI.SetTile(r, i, j, 1, mapMaker.sand)
			else
				regionAPI.SetTile(r, i, j, 1, mapMaker.water)
				regionAPI.SetSolid(r, i, j, true)
			end
		end
	end

	--examples of the smoothing function NOT working correctly TODO: fix the smoothing system
	--[[
	for j = 1, regionAPI.GetHeight(r) do
		regionAPI.SetTile(r, 3, j, 1, mapMaker.dirt)
		regionAPI.SetTile(r, 4, j, 1, mapMaker.dirt)

		regionAPI.SetTile(r, 10, j, 1, mapMaker.dirt)
	end
	--]]

	--A generic edge system
	mapMaker.SmoothEdgesSimple(r)
end

function mapMaker.DebugGrassland(r)
	--debug
	io.write("map_maker:DebugGrassland(", regionAPI.GetX(r), ", ", regionAPI.GetY(r), ")\n")

	--all grass, ring of water
	for i = 1, regionAPI.GetWidth(r) do
		for j = 1, regionAPI.GetHeight(r) do
			local dist = mapMaker.Dist(0, 0, i + regionAPI.GetX(r) -1, j + regionAPI.GetY(r) -1)

			if dist < 50 then
				regionAPI.SetTile(r, i, j, 1, mapMaker.grass)
			elseif dist < 55 then
				regionAPI.SetTile(r, i, j, 1, mapMaker.water)
				regionAPI.SetSolid(r, i, j, true)
			else
				regionAPI.SetTile(r, i, j, 1, mapMaker.grass)
			end
		end
	end

	--A generic edge system
--	mapMaker.SmoothEdgesSimple(r)
end

return mapMaker