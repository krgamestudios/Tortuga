local region = require("region")

local mapSaver = {}

function mapSaver.Load(r)
	--empty
--	io.write("map_saver:Load(", region.GetX(r), ", ", region.GetY(r), ")\n")
end
function mapSaver.Save(r)
	--empty
--	io.write("map_saver:Save(", region.GetX(r), ", ", region.GetY(r), ")\n")
end

--TODO: (3) create a flexible saving & loading system
return mapSaver