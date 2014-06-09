print("Lua script check (./rsc)")

-------------------------
--Map API overrides
-------------------------

function region.create(r)
	for i = 1, region.getwidth() do
		for j = 1, region.getheight() do
			if math.abs(region.getx(r) + i -1) == math.abs(region.gety(r) + j -1) then
				region.settile(r, i, j, 1, 50)
			else
				region.settile(r, i, j, 1, 14)
			end
		end
	end

	--signal
	region.settile(r, 4, 5, 2, 86)
end
