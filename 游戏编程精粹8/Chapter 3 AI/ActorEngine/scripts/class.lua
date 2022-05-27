-- A barebones single inheritance class
-- system with syntactic sugar
--[[ usage

	class "Entity"{
		position=function()
			--stuff
		end
	}

	class "Character":inherit "Entity"{
		walk=function(self,goal)
			--stuff
		end
	}

	class "Player":inherit "Character"{
		jump=function(self,target)
			--stuff
		end,
		walk=function(self,goal) --overload
			--stuff
		end
	}

	playerclass = getclass("Player") --> Player class metatable, not and object
]]

local pairs = pairs
local assert = assert
local setmetatable = setmetatable

----------------
--private API
local __classes = {}

local function __builder(self,def)
	local k,v
	for k,v in pairs(def) do
		self.methods[k]=v
	end
	return self
end

----------------
--public API
function inherit(self,classname)
	return self(assert(	__classes[classname]
					and __classes[classname].methods,
					"unknown parent class"))
end

function class(classname)
	local c = setmetatable({
					classname=classname,
					methods={},
					inherit=inherit},
					{__call=__builder})
	__classes[classname]=c
	return c
end

function getclass(classname)
	return __classes[classname]
end
