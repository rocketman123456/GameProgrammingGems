log("send a simple message")

class "Ping" {
	init = function(self,msg)
		log("ping")
		local a = getactor("pong")
		a:pong()
		die()
	end
}

class "Pong" {
	pong = function(self,msg)
		log("pong")
		die()
	end
}

local ping = spawnactor("ping","Ping")
local pong = spawnactor("pong","Pong")
