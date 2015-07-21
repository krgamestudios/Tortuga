--args are: userdata RegionPagerLua;

print("Running startup script")

pager = ...

--[[

--DOCS: These lambdas should return true or false, depending on if the operation succeeded or not
--DOCS: No return value given is recognized as a failure
--DOCS: OnCreate() and OnUnload() return values are currently ignored
region_pager.SetOnLoad(pager, function(r)
	print("Calling SetOnLoad's lambda")
end)
region_pager.SetOnSave(pager, function(r)
	print("Calling SetOnSave's lambda")
end)
region_pager.SetOnCreate(pager, function(r)
	print("Calling SetOnCreate's lambda")
end)
region_pager.SetOnUnload(pager, function(r)
	print("Calling SetOnUnload's lambda")
end)

--]]

generator, msg = loadfile("rsc/roguegenerator.lua")

if generator == nil then
	print("error: ", msg)
else
	generator(pager)
end

print("Finished startup script")