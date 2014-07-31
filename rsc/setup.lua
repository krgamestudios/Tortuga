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
	--common stuff
	debug = debug,
	dir = {
		fonts = "rsc/graphics/fonts/",
		logos = "rsc/graphics/logos/",
		sprites = "rsc/graphics/sprites/",
		tilesets = "rsc/graphics/tilesets/",
		interface = "rsc/graphics/interface/",
		scripts = "rsc/scripts/",
		maps = "rsc/maps/"
	},

	--client specific stuff
	client = {
		screen = {
			width = 800,
			height = 600,
			fullscreen = false
		},
		username = "Kayne",
		handle = "Ratstail91",
		avatar = "elliot2.bmp",

		--NOTE: these generally go here
--		clientIndex
--		accountIndex
--		characterIndex
	},

	--server specific stuff
	server = {
		mapname = "mapsave",
		host = "255.255.255.255",
		port = 21795,
		name = "local",
		dbname = "database.db"
	}
}

--development environment
if config.debug then
	for k, v in pairs(config.dir) do
		config.dir[k] = string.format("../%s", v)
	end
end

