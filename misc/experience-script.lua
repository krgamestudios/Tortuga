function CalcExp(base, mod, level)
	return math.floor(base * mod ^ level)
end

function CalcSumExp(base, mod, level)
	local ret = CalcExp(base, mod, level)
	if (level > 1) then
		return ret + CalcSumExp(base, mod, level-1)
	else
		return ret
	end
end

function CalcExpTables(base, mod, levelCap)
	local exp = {}
	local sum = {}
	for i = 1, levelCap do
		exp[i] = CalcExp(base, mod, i)
		sum[i] = CalcSumExp(base, mod, i)
	end
	return exp, sum
end

--the variables
base		= 1000
mod			= 1.14
levelCap	= 20

--calc the tables
exp, sum = CalcExpTables(base, mod, levelCap)

--output the data
io.write("Level\t\tExp\t\tTotal\t\tLaps\n")
for i = 1, 60 do io.write("-") end
io.write("\n")

for i = 1, levelCap do
	if i % 5 == 0 then
		sep = "\t---\t"
	else
		sep = "\t\t"
	end
	io.write(i,
		sep, exp[i],
		sep, sum[i],
		sep, math.floor(sum[i] / sum[1] * 10) / 10, --rounded
--		sep, sum[i] / sum[1], --real
		"\n")
end
