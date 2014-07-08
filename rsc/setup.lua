--[[
	--reroute the program while in development
	config = {debug = true}
	dofile("../rsc/setup.lua")
--]]

--catch the debug signal if the program was rerouted
local debug = false
if config ~= nil then
	debug = config.debug
end

--the program's configuration
config = {
	debug = debug,
	screen = {
		width = 800,
		height = 600,
		fullscreen = false
	},
	dir = {
		fonts = "rsc/graphics/fonts/",
		logos = "rsc/graphics/logos/",
		sprites = "rsc/graphics/sprites/",
		tilesets = "rsc/graphics/tilesets/",
		interface = "rsc/graphics/interface/",
		scripts = "rsc/scripts/",
		maps = "rsc/maps/"
	},
	mapname = "mapname"
}

--development environment
if config.debug then
	for k, v in pairs(config.dir) do
		config.dir[k] = string.format("../%s", v)
	end
end

--"load" the scripted systems
dofile(config.dir.scripts .. "file_format.lua")
dofile(config.dir.scripts .. "generator.lua")