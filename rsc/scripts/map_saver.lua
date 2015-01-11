local Region = require("map_system").Region

local mapSaver = {}

function mapSaver.Load(r)
	--empty
	io.write("map_saver:Load(", Region.GetX(r), ", ", Region.GetY(r), ")\n")
end
function mapSaver.Save(r)
	--empty
	io.write("map_saver:Save(", Region.GetX(r), ", ", Region.GetY(r), ")\n")
end

--TODO: create a flexible saving & loading system
return mapSaver