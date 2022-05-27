--Utilities
local type = type
local error = error
local assert = assert

require"actor"
local _currentactor = currentactor

local A = assert(actorlib)
local _send = assert(A.send)
local _send_exception = assert(A.sendexception)

function noop()
	--noop
end

--logger
function log(msg,...)
	if type(msg)=="string" then
		_send("Logger",{log=msg:format(...)})
	else
		_send("Logger",msg)
	end
end

--timer
function starttimer(actor, frequency, cycle)
	assert(actor and frequency,"arguments error: (actor, frequency [, cycle])")
	_send("Clock",{register=actor.__address,frequency=frequency,cycle=true})
end

function stoptimer(actor)
	assert(actor,"arguments error: (actor)")
	_send("Clock",{unregister=actor.__address})
end

--misc
function help()
	_send("Root",{call="help"})
end

function exit() --clean quit, when current work queue is done
	_send_exception("Root",{error="exit"})
end

function die(actor) --dirty quit, stop all work now and quit
	(actor or _currentactor()){error="die"}
end
