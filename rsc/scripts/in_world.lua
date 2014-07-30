local sheet = TileSheet.GetTileSheet()
local pager = RegionPager.GetRegionPager()

--the selected tilesheet
TileSheet.Load(sheet, config.dir.tilesets .. "terrain.bmp", 32, 32)

--tile macros, mapped to this tilesheet
local base = 14
local shift = 36
tiles = {
	plains	= base + shift * 0,
	grass	= base + shift * 1,
	dirt	= base + shift * 2,
	sand	= base + shift * 3,
	water	= base + shift * 4
}

--could set custom generation systems here, that differ from the global generators, etc.
function Region.Create(region)
	for i = 1, Region.GetWidth() do
		for j = 1, Region.GetHeight() do
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
