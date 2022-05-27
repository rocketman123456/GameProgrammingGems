--Main script
package.path=package.path..";scripts/?.lua"

require"class"
require"actor"
require"utils"

--load game script
local ok,err = pcall(dofile,"scripts/game.lua")
if not ok then
	print(err)
end

local A = assert(actorlib)
local _getproperty = assert(A.getproperty)

local _currentactor = assert(currentactor)

--entry point for messages
function receive(msg)
	local classname = _getproperty("class") or _getproperty("name")
	local self = _currentactor(classname)
	return self[msg.call or msg.init or "receive"](self,msg)
end

--entry point for exceptions
function exception(msg)
	local classname = _getproperty("class") or _getproperty("name")
	local exceptionhandler = _currentactor(classname).exception
	if exceptionhandler then
		return exceptionhandler(self,msg)
	else
		log(msg)
		return false
	end
end

