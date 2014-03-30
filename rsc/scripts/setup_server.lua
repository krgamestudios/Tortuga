print("Lua script check OK (./rsc)")

function Region.Create(r)
	print("Region:Create(r", Region.GetX(r), Region.GetY(r), ")")
end

function Region.Unload(r)
	print("Region:Unload(r", Region.GetX(r), Region.GetY(r), ")")
end

--return true if file loaded, otherwise return false
function Region.Load(r, saveDir)
	print("Region:Load(r,", saveDir, Region.GetX(r), Region.GetY(r), ")")
	return false
end

function Region.Save(r, saveDir)
	print("Region:Save(r,", saveDir, Region.GetX(r), Region.GetY(r), ")")
end
