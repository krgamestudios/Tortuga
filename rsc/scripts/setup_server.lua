print("Lua script check")

--uber lazy declarations
function math.sqr(x) return x*x end
function math.dist(x, y, i, j) return math.sqrt(math.sqr(x - i) + math.sqr(y - j)) end

--tile macros, mapped to the tilesheet
local base = 14
local shift = 36
tiles = {
	plains	= base + shift * 0,
	grass	= base + shift * 1,
	dirt	= base + shift * 2,
	sand	= base + shift * 3,
	water	= base + shift * 4
}

--custom generation systems here
function islandGenerator(region)
	for i = 1, Region.GetWidth(region) do
		for j = 1, Region.GetHeight(region) do
			local dist = math.dist(0, 0, i + Region.GetX(region) -1, j + Region.GetY(region) -1)
			if dist < 10 then
				Region.SetTile(region, i, j, 1, tiles.plains)
			elseif dist < 12 then
				Region.SetTile(region, i, j, 1, tiles.sand)
			else
				Region.SetTile(region, i, j, 1, tiles.water)
				Region.SetSolid(region, i, j, true)
			end
		end
	end
end

--Get some regions
--BUG: The server fails without at least one room
--TODO: Create rooms with names?
newRoom = RoomManager.CreateRoom()
pager = Room.GetPager(newRoom)
RegionPager.SetOnCreate(pager, islandGenerator)

--[[
regionTable = {
	RegionPager.GetRegion(pager, Region.GetWidth() * 0, Region.GetHeight() * 0),
	RegionPager.GetRegion(pager, Region.GetWidth() *-1, Region.GetHeight() * 0),
	RegionPager.GetRegion(pager, Region.GetWidth() * 0, Region.GetHeight() *-1),
	RegionPager.GetRegion(pager, Region.GetWidth() *-1, Region.GetHeight() *-1)
}
]]

print("Finished the lua script")
