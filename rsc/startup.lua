--args are: userdata RegionPagerLua;

print("Running startup script")

pager = ...

---[[

--BUG: RegionPagerLua fails without these
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

print("Finished startup script")