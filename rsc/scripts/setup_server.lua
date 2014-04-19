print("Lua script check OK (./rsc)")

function Region.Create(r)
--	print("Region:Create(r", Region.GetX(r), Region.GetY(r), ")")
	for i = 1, Region.GetWidth(r) do
		for j = 1, Region.GetHeight(r) do
			if math.abs(i) == math.abs(j) then
				Region.SetTile(r, i, j, 1, 50)
			else
				Region.SetTile(r, i, j, 1, 14)
			end
		end
	end
--	print("done")
end

function Region.Unload(r)
--	print("Region:Unload(r", Region.GetX(r), Region.GetY(r), ")")
end

--return true if file loaded, otherwise return false
function Region.Load(r, saveDir)
--	print("Region:Load(r,", saveDir, Region.GetX(r), Region.GetY(r), ")")
	return false
end

function Region.Save(r, saveDir)
--	print("Region:Save(r,", saveDir, Region.GetX(r), Region.GetY(r), ")")
end
