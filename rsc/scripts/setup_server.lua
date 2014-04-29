print("Lua script check OK (./rsc)")

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

--return true if file loaded, otherwise return false
function map.load(region, dir)
	--
	return false
end

function map.save(region, dir)
	--
end

--debugging
print("DEBUG: Initial tile value: ", map.gettile(0, 0, 0))
map.settile(0, 0, 0, 86)
map.settile(10, 10, 1, 156)
