-------------------------
-- the Root actor of the Console
class "Root"{
	--This serves as the main entry point
	init=function(self)
		local Input = getactor("Input")
			starttimer(Input,1/30,true) --input frequency

		local Controller = spawnactor("Controller")
			Input{register="Controller"}

		log("Root initialized")
		--display help message
		self:help()
	end,
	exception=function(self,msg)
		if msg.error=="exit" then
			--we want to exit
			return false
		elseif msg.error=="die" then
			--some actor requested to die
			return true -- handled
		else
			--some exception bubbled it's way here
			log(msg)
			return true -- handled
		end
	end,
	help=function(self)
		log([[
		This is a scriptable actor console.
		usage:
			press 'h' to display this message
			press 'r' to reload game script
			press ENTER to begin console mode
				within console mode press ENTER after an
				empty line to return to	async mode
			press 1~9 to run samples
			press 'ESC' to quit
		]])
	end,
}

-------------------------
-- the Controller actor handles input
-- originated fron the Input actor
local keys --callback table
class "Controller"{
	receive=function(self,msg)
		-- Interactive console mode
		if msg.cmd then
			local buffer = self:get_buffer() or ""
			if #msg.cmd>0 then
				buffer = ("%s\n%s"):format(buffer,msg.cmd)
			else
				if #buffer==0 then --leave console mode
					log("entering key mode")
					local Input = getactor("Input")
					Input{mode="async"}
				else --execute console
					local f,err = loadstring(buffer)
					if f then
						local ok, err = pcall(f)
						if not ok then
							log(err)
						end
					else
						log(err)
					end
				end
				buffer = nil
			end
			self:set_buffer(buffer)
		end
		-- Async key press mode
		if msg.key then
			local keyhandler = keys[msg.key] or keys[string.char(msg.key)]
			if keyhandler then
				return keyhandler(msg)
			else
				-- unbound key
				log("key:"..("%q"):format(string.char(msg.key)).."["..msg.key.."]")
			end
		end
	end,
	exception=function(self,msg)
		-- don't let the Controller die
		log(msg)
		return true
	end
}

-- bind key callbacks
keys = {
-- help
	['h'] = help,
-- reload script
	['r'] = function()
		log("reloading game script")
		dofile("scripts/game.lua")
	end,
-- start console mode [ENTER]
	["\r"] = function()
		log("entering console mode")
		local Input = getactor("Input")
		Input{mode="sync"}
	end,
-- exit [ESC]
	[27] = exit
}
-- tests [1..9]
local function test_loader(n)
	local n = n
	return function()
		log("loading test "..n)
		local ok,err = pcall(dofile,"scripts/test_"..n..".lua")
		if not ok then
			log("error in test "..n.." -> "..err)
		end
	end
end
for i=1,9 do
	keys[tostring(i)] = test_loader(i)
end

log("loaded game.lua")
