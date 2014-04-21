--NOTE: This is the outline for the final combat system

--enumeration
hitType = { MISS = 1, HIT = 2, CRITICAL = 3 }
attackType = { PHYSICAL = 1, MAGICAL = 2 }

function MakeChar()
	local t = {}
	t.MaxHP = 100
	t.HP = t.MaxHP
	t.MaxMP = 100
	t.MP = t.MaxMP
	t.attack = 10
	t.defence = 10
	t.intelligence = 10
	t.resistance = 10
	t.accuracy = 0.1
	t.evasion = 0.1
	t.luck = 0.1
	t.level = 1
	return t
end

function CheckHit(A, D)
	local RNG = math.random(100)
	local hitRate = A.accuracy - A.accuracy * D.evasion

	if RNG <= hitRate * A.luck then
		return hitType.CRITICAL
	end
	if RNG <= hitRate then
		return hitType.HIT
	end
	return hitType.MISS
end

function CalcDamage(A, D, t)
--	if t == attackType.PHYSICAL then
		return math.max(1, ( A.attack*3 + A.attack*(math.random(A.luck*100)/10) ) - (D.defence + D.defence*D.evasion))
--	end
--	if t == attackType.MAGICAL then
--		--??
--	end
--	--wtf?
end

A = MakeChar()
D = MakeChar()

dump = io.open("dump.txt", "w")

--top
dump:write("\t\t")
for i = 10, 100, 10 do
	dump:write(i, "\t")
end
dump:write("\n\n")

--lines
for j = 10, 100, 10 do
	dump:write(j, "\t\t")
	for i = 10, 100, 10 do
		A.attack = i
		D.defence = j
		dump:write(CalcDamage(A, D, 0), "\t")
	end
	dump:write("\n")
end

--bottom
dump:write("\n")
dump:close()