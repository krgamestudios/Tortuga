--uber lazy declarations
function math.sqr(x) return x*x end
function math.dist(x, y, i, j) return math.sqrt(math.sqr(x-i) + math.sqr(y-j)) end

--define these
function Region.Create(region)
	for i = 1, Region.GetWidth() do
		for j = 1, Region.GetHeight() do
			Region.SetTile(region, i, j, 1, 1) --to show the basics are working
		end
	end
end

function Region.Unload(region)
	--
end
