local doorUtility = {}

roomAPI = require("room")
regionPagerAPI = require("region_pager")

triggerAPI = require("trigger")
triggerManagerAPI = require("trigger_manager")

entityAPI = require("entity")
characterAPI = require("character")

networkAPI = require("network")

function doorUtility.createTrigger(handle, room, x, y, script)
	local pager = roomAPI.GetPager(room)

	--place the indicator tile
	regionPagerAPI.SetTile(pager, x / 32, y / 32, 0, mapMaker.dirt)
	regionPagerAPI.SetTile(pager, x / 32, y / 32, 1, mapMaker.blank)
	regionPagerAPI.SetTile(pager, x / 32, y / 32, 2, mapMaker.blank)

	--create the trigger object
	triggerManagerAPI.Create(
		roomAPI.GetTriggerMgr(room), handle, x, y,
		0, 0, 32, 32, --size of the tiles
		script
		)
end

function doorUtility.createDoorPair(handle, roomOne, Xone, Yone, roomTwo, Xtwo, Ytwo)
	--create the scripts
	local function scriptOne(entity)
		if entityAPI.GetType(entity) ~= "character" then return end

		--move the character
		characterAPI.SetRoom(entity, roomTwo)
		characterAPI.SetOrigin(entity, Xtwo, Ytwo-16)
		networkAPI.PumpCharacterUpdate(entity)

		--disable the other trigger
		local triggerTwo = triggerManagerAPI.GetTrigger(roomAPI.GetTriggerMgr(roomTwo), handle)
		triggerAPI.PushExclusionEntity(triggerTwo, entity)
	end

	local function scriptTwo(entity)
		if entityAPI.GetType(entity) ~= "character" then return end

		--move the character
		characterAPI.SetRoom(entity, roomOne)
		characterAPI.SetOrigin(entity, Xone, Yone-16) --NOTE: the 16 pixel margin for presentation
		networkAPI.PumpCharacterUpdate(entity)

		--disable the other trigger
		local triggerOne = triggerManagerAPI.GetTrigger(roomAPI.GetTriggerMgr(roomOne), handle)
		triggerAPI.PushExclusionEntity(triggerOne, entity)
	end

	--create the triggers proper
	doorUtility.createTrigger(handle, roomOne, Xone, Yone, scriptOne)
	doorUtility.createTrigger(handle, roomTwo, Xtwo, Ytwo, scriptTwo)
end

return doorUtility