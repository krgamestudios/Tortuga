print("Lua script check")

--uber lazy declarations
function square(x) return x*x end
function distance(x, y, i, j) return math.sqrt(square(x - i) + square(y - j)) end

--tile macros, mapped to the tilesheet
local base = 14
local shift = 36
plains	= base + shift * 0
grass	= base + shift * 1
dirt	= base + shift * 2
sand	= base + shift * 3
water	= base + shift * 4

--Overwrite the original OnCreate with my own version
Region.hcOnCreate = Region.OnCreate
Region.OnCreate = function(region)
	local ret = Region.hcOnCreate(region) --best practices
	for i = 1, Region.GetWidth() do
		for j = 1, Region.GetHeight() do
			if distance(0, 0, i + Region.GetX(region) -1, j + Region.GetY(region) -1) > 10 then
				Region.SetTile(region, i, j, 1, water)
			else
				Region.SetTile(region, i, j, 1, plains)
			end
		end
	end
	return ret
end

--Get some regions
newRoom = RoomMgr.CreateRoom("overworld")
pager = Room.GetPager(newRoom)
regionTable = {
	RegionPager.GetRegion(pager, Region.GetWidth() * 0, Region.GetHeight() * 0),
	RegionPager.GetRegion(pager, Region.GetWidth() *-1, Region.GetHeight() * 0),
	RegionPager.GetRegion(pager, Region.GetWidth() * 0, Region.GetHeight() *-1),
	RegionPager.GetRegion(pager, Region.GetWidth() *-1, Region.GetHeight() *-1)
}

print("Finished the lua script")