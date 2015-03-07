print("Lua script check")

mapMaker = require("map_maker")
mapSaver = require("map_saver")
mapSystem = require("map_system")
roomSystem = require("room_system")
characterSystem = require("character_system")
networkSystem = require("network")
triggerSystem = require("trigger_system")

local function dumpTable(t)
	print(t)
	for k, v in pairs(t) do
		print("",k,v)
	end
end

--test the room hooks
roomSystem.RoomManager.SetOnCreate(function(room, index)
	print("", "Creating room: ", roomSystem.Room.GetName(room), index)

	--called ~60 times per second
--	roomSystem.Room.SetOnTick(room, function(room)
--		roomSystem.Room.ForEachCharacter(room, function(character)
--			print(characterSystem.Character.GetHandle(character))
--		end)
		--[[
		local character = characterSystem.CharacterManager.GetCharacter("handle")
		if character ~= nil then
			--debugging
			local originX, originY = characterSystem.Character.GetOrigin(character)
			local motionX, motionY = characterSystem.Character.GetMotion(character)
			if motionY < 0 then
				characterSystem.Character.SetMotion(character, motionX, 0)
				networkSystem.PumpCharacterUpdate(character)
				print("Sending: ", motionX, motionY)
			end
		end
		--]]
--	end)
end)

roomSystem.RoomManager.SetOnUnload(function(room, index)
	print("", "Unloading room: ", roomSystem.Room.GetName(room), index)
end)

--NOTE: room 0 is the first that the client asks for, therefore it must exist
local overworld, uid = roomSystem.RoomManager.CreateRoom("overworld", "overworld.bmp")
roomSystem.Room.Initialize(overworld, mapSaver.Load, mapSaver.Save, mapMaker.DebugIsland, mapSaver.Save)

--debug: test the trigger system
local pager = roomSystem.Room.GetPager(overworld)
mapSystem.RegionPager.SetTile(pager, 0, 0, 0, mapMaker.dirt)
local triggerMgr = roomSystem.Room.GetTriggerMgr(overworld)
--TODO: (1) What does the trigger script take as a parameter?
triggerSystem.TriggerManager.Create(triggerMgr, "dirt", 0, 0, 0, 0, 32, 32, function(character)
	local x, y = characterSystem.Character.GetOrigin(character)
	characterSystem.Character.SetOrigin(character, x, y + 128)
	networkSystem.PumpCharacterUpdate(character)
end)

--debugging
function dumpTrigger(t)
	local originX, originY = triggerSystem.Trigger.GetOrigin(t)
	local bx, by, bw, bh = triggerSystem.Trigger.GetBounds(t)
	local s = triggerSystem.Trigger.GetScript(t)
	print(triggerSystem.Trigger.GetHandle(t), originX, originY, bx, by, bw, bh, s)
end

function dumpTable(t)
	for k, v in pairs(t) do
		print("", k, v)
	end
end

--[[
triggerMgr = roomSystem.Room.GetTriggerMgr(overworld)
trigger1, uid1 = triggerSystem.TriggerManager.Create(triggerMgr, "handle1")
trigger2, uid2 = triggerSystem.TriggerManager.Create(triggerMgr, "handle2", 30.2, 40.2)
trigger3, uid3 = triggerSystem.TriggerManager.Create(triggerMgr, "handle3", 30.2, 40.2, 0, 16, 32, 32, function() end)

dumpTrigger(trigger1)
dumpTrigger(trigger2)
dumpTrigger(trigger3)
--]]

print("Finished the lua script")
