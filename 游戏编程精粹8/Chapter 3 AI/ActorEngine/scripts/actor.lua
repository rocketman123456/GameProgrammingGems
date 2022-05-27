--Functionality to wrap actors for a friendlier API
--[[ usage

	local player = getactor("player1","Player")
	local enemy = getactor("goblin01")
	player:attack(enemy)

	promiseHp = enemy:get_hp()
	if promiseHp() > 50 then –-invoking the promise returns its value
		player:run()
	end

]]
--assert that the required API is defined correctly
local A = assert(actorlib)
local _getproperty = assert(A.getproperty)
local _setproperty = assert(A.setproperty)
local _send = assert(A.send)
local _send_exception = assert(A.sendexception)
local _spawn = assert(A.spawn)
--local _makepromise = assert(A.makepromise)
--local _claimpromise = assert(A.claimpromise)

local type = type
local pairs = pairs
local assert = assert
local setmetatable = setmetatable
local tostring = tostring
local error = error
local rawget = rawget

require"class"
local getclass = assert(getclass)

local mt_currentactor = {}
local mt_classactor = {}
local mt_genericactor = {}

----------------
--public API
function currentactor(classname)
	return setmetatable({__address=_getproperty("name"),__class=getclass(classname)}, mt_currentactor)
end

function getactor(address,classname)
	local class = getclass(classname or address)
	if class then
		return setmetatable({__address=address, __class=class}, mt_classactor)
	else
		return setmetatable({__address=address }, mt_genericactor)
	end
end

function spawnactor(address,classname)
	_spawn(address,classname)
	return getactor(address,classname)
end

----------------
--private API

--utilities
local generic_calls={
	receive = function()
		return
	end,
	exception = function()
		return false
	end,
	init = function()
		return
	end
}
local function memoize (f)
	local mem = setmetatable({}, {__mode = "kv"})
	return function(x)
		local r = mem[x]
		if r==nil then
			r = f(x)
			mem[x] = r
		end
		return r
	end
end

--basic operations
local send = memoize(function(key)
	return function(target,...)
		_send(target.__address,{call=key,...})
	end
end)

local get = memoize(function(key)
	return function(self)
		local ret
		return function()
			ret = ret or _getproperty(key)
			return ret
		end
	end
end)

local getpromise = memoize(function(key)
	return function(target)
		local callId = _makepromise(target.__address,{call=key})
		return function()
			local val = _claimpromise(callId)
			return val and val.val or nil
		end
	end
end)

local set = memoize(function(key)
	return function(self,val)
		if type(val)=="table" then
			val = val.val
		end
		_setproperty(key,val)
	end
end)

local setmessage = memoize(function(key)
	return function(target,val)
		_send(target.__address,{call=key,val=val})
	end
end)

local mod = memoize(function(key)
	return function(self,val)
		if type(val)=="table" then
			val = val.val
		end
		local old = _getproperty(key)
		_setproperty(key,old+val)
	end
end)

local modmessage = memoize(function(key)
	return function(target,val)
		_send(target.__address,{call=key,val=val})
	end
end)

--wrapper for an actor with a defined class
mt_classactor_properties = memoize(function(key)
	if key:find("get_")==1 then
		error("promises are not supported in this version")
		--return getpromise(key)
	elseif key:find("set_")==1 then
		return setmessage(key)
	elseif key:find("mod_")==1 then
		return modmessage(key)
	end
end)

mt_classactor.__index = function(self,key)
	assert(type(key)=="string","cannot handle index : "..tostring(key) )
	local ret = self.__class.methods[key] and send(key) or mt_classactor_properties(key) or generic_calls[key]
	assert(ret,"unknown method : "..tostring(key))
	self[key] = ret --memoize
	return ret
end

mt_classactor.__call = function(self,msg)
	_send(self.__address,msg)
end

--wrapper for an actor without a defined class
mt_genericactor_properties = memoize(function(key)
	if key:find("get_")==1 then -- property promises
		error("promises are not supported in this version")
		--return getpromise(key)
	elseif key:find("set_")==1 then
		return setmessage(key)
	elseif key:find("mod_")==1 then
		return modmessage(key)
	end
end)

mt_genericactor.__index = function(self,key)
	assert(type(key)=="string","cannot handle index : "..tostring(key) )
	local ret = mt_genericactor_properties(key) or send(key) or generic_calls[key]
	assert(ret,"unknown method : "..tostring(key))
	self[key] = ret --memoize
	return ret
end

mt_genericactor.__call = function(self,msg)
	_send(self.__address,msg)
end

--wrapper for the current actor that is being processed
mt_currentactor_properties = memoize(function(key)
	if key:find("get_")==1 then
		return get(key:sub(5,-1))
	elseif key:find("set_")==1 then
		return set(key:sub(5,-1))
	elseif key:find("mod_")==1 then
		return mod(key:sub(5,-1))
	end
end)

mt_currentactor.__index = function(self,key)
	assert(type(key)=="string","cannot handle index : "..tostring(key) )
	local ret
	local class = rawget(self,"__class")
	if class then
		ret = class.methods[key]
	end
	if not ret then
		ret = mt_currentactor_properties(key) or generic_calls[key]
	end
	assert(ret,"unknown method : "..tostring(key))
	self[key] = ret --memoize
	return ret
end

mt_currentactor.__call = function(self,msg)
	local _ = self.receive and self.receive(msg)
end

