print("Lua script check (./rsc)")

-------------------------
--Map API overrides
-------------------------

function map.create(region)
	for i = 1, map.getregionwidth() do
		for j = 1, map.getregionheight() do
			if math.abs(map.getx(region) + i -1) == math.abs(map.gety(region) + j -1) then
				map.settile(region, i, j, 1, 50)
			else
				map.settile(region, i, j, 1, 14)
			end
		end
	end
end

function map.unload(region)
	--
end

function map.load(region, dir)
	--return true if file loaded, otherwise return false
	return false
end

function map.save(region, dir)
	--
end

-------------------------
--Enemy API
-------------------------

--TODO