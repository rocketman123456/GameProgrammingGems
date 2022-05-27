/****************************************************************************
* Copyright (c) 2009 Robert Jay Gould, robert.jay.gould@gmail.com
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*****************************************************************************/

/****************************************************************************
*	A multithreaded actor-based Lua console.
*	the latest version can be found at:
*		http://code.google.com/p/tbb-actor/
*****************************************************************************/

#pragma once

#include<map>
#include<tbb/enumerable_thread_specific.h>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include"PropertyMessage.hpp"
#include"SimpleActor.hpp"

class bad_lua_handling : public tbb::movable_exception<void*>
{
public:
	bad_lua_handling()
		:tbb::movable_exception<void*>(NULL)
	{}
	virtual const char* what() const throw()
	{
		return "invalid parameter type set by lua";
	}
};

class bad_lua_property : public bad_lua_handling
{
public:
	bad_lua_property()
		:bad_lua_handling()
	{}
	virtual const char* what() const throw()
	{
		return "invalid parameter type set by lua";
	}
};

class lua_load_error : public bad_lua_handling
{
public:
	lua_load_error()
		:bad_lua_handling()
	{}
	virtual const char* what() const throw()
	{
		return "failed to load lua script";
	}
};

//forward declaration
int luaopen_alib (lua_State *L);

class LuaVM
{
	lua_State*			L;
	bool				initialized;

public:
	static std::string	mainscript;

	LuaVM()
		: L(NULL)
		, initialized(false)
	{
		//noop
	}

	void open()
	{

	}

	void close()
	{
		if(L)
		{
			lua_close(L);
			L = NULL;
		}
	}

	bool receive(SimpleActor::msg_t& msg)
	{
		if(!initialized)
		{
			L = luaL_newstate();
			assert(L);
			luaL_openlibs(L);
			luaopen_alib(L);
			int err = luaL_dofile(L,mainscript.c_str());
			if(err)
			{
				std::cerr << lua_tostring(L, -1) << std::endl;
			}
			else
			{
				lua_pushstring(L,mainscript.c_str());
				lua_setglobal(L,"_MAIN");
			}
			lua_settop(L,0);
			initialized = true;
		}
		if(msg.has("error"))
		{
			lua_getglobal(L, "exception");
		}
		else
		{
			lua_getglobal(L, "receive");
		}
		bool	ok		=	true;
		//pack message for Lua
		push_message(L,msg);
		int err = lua_pcall(L,1,1,0);
		if(err)
		{
			msg.properties["error"] = lua_tostring(L, -1);
			ok = false;
		}
		else
		{
			if(lua_isboolean(L,-1))
			{
				ok = lua_toboolean(L,-1)!=0;
				if(!ok)
				{
					if(!msg.has("error"))
					{
						msg.properties["error"] = "unknown handling error";
					}
				}
			}
		}
		lua_settop(L,0);
		return ok;
	}

	static void push_message(lua_State* L, SimpleActor::msg_t& msg)
	{
		lua_newtable(L);
		SimpleActor::msg_t::iterator iter = msg.properties.begin();
		SimpleActor::msg_t::iterator end = msg.properties.end();
		for(;iter!=end;++iter)
		{
			lua_pushstring(L,iter->first.c_str());//name
			switch(iter->second.get_type())//value
			{
			case property_types::NIL :
				{
					lua_pushnil(L);
				} break;
			case property_types::BOOLEAN :
				{
					lua_pushboolean(L,iter->second.get_boolean());
				} break;
			case property_types::NUMBER :
				{
					lua_pushnumber(L,iter->second.get_number());
				} break;
			case property_types::STRING :
				{
					lua_pushstring(L,iter->second.get_string().c_str());
				} break;
			}
			lua_rawset(L,-3);//t[name]=value
		}
	}

	static void get_message(lua_State* L, std::string& target, SimpleActor::msg_t& msg)
	{
		target					= luaL_checkstring(L,1);
		int nret				= lua_gettop(L);
		for(int iter=2;iter<=nret;++iter)
		{
			std::stringstream paramName;
			paramName << iter-1;
			switch(lua_type(L,iter))
			{
			case LUA_TNIL:
				{
					msg.properties.erase(paramName.str());
					break;
				}
			case LUA_TNUMBER:
				{
					msg.properties[paramName.str()] = lua_tonumber(L,iter);
					break;
				}
			case LUA_TBOOLEAN:
				{
					msg.properties[paramName.str()] = (lua_toboolean(L,iter)!=0);
					break;
				}
			case LUA_TSTRING:
				{
					msg.properties[paramName.str()] = lua_tostring(L,iter);
					break;
				}
			case LUA_TTABLE:
				{
					lua_pushnil(L);
					while (lua_next(L, iter) != 0)
					{
						// 'key' at -2 and 'value' at -1
						if(lua_type(L, -2)==LUA_TSTRING || lua_type(L, -2)==LUA_TNUMBER) //hash
						{
							switch(lua_type(L,-1))
							{
							case LUA_TNUMBER:
								{
									msg.properties[lua_tostring(L,-2)] = lua_tonumber(L,-1);
									break;
								}
							case LUA_TBOOLEAN:
								{
									msg.properties[lua_tostring(L,-2)] = (lua_toboolean(L,-1)!=0);
									break;
								}
							case LUA_TSTRING:
								{
									msg.properties[lua_tostring(L,-2)] = lua_tostring(L,-1);
									break;
								}
							default:
								{
									lua_settop(L,0);
									throw bad_lua_property();
								}
							}
						}
						lua_pop(L, 1);
					}
				} break;
			default:
				{
					lua_settop(L,0);
					throw bad_lua_property();
				}
			}
		}
	}
};

//global main script name
std::string LuaVM::mainscript;

//thread local storage
typedef tbb::enumerable_thread_specific<LuaVM>	LuaVMPool_t;
LuaVMPool_t									LuaVMPool;

class LuaActor;
typedef tbb::enumerable_thread_specific<LuaActor*> Currentactor_t;
Currentactor_t								Currentactor;


//Responsability: Generic scriptable actor
class LuaActor : public SimpleActor
{
protected:
	property_map						properties;
	LuaActor*							previousactor;

public:
	LuaActor(const std::string& name, SimpleActor* parent = NULL)
		: SimpleActor(name,parent)
	{
		properties["name"] = name;
	}

	virtual ~LuaActor()
	{
		//noop
	}

	void pre_receive()
	{
		//set self as current actor for the thread
		Currentactor_t::reference ref = Currentactor.local();
		previousactor = ref; ref = this;//swap
		LuaVMPool_t::reference my_vm = LuaVMPool.local();
		//keep pointer to local VM
		vm = &my_vm;
	}

	void post_receive()
	{
		Currentactor_t::reference ref = Currentactor.local();
		ref = previousactor; //replace previous actor
	}

	virtual bool receive(msg_t& msg)
	{
		bool res = vm->receive(msg);
		return res;
	}

	virtual bool receive_exception(tbb::tbb_exception& e, msg_t& msg)
	{
		if(!receive(msg))
		{
			AddressSpace::send_exception(this,"Logger",e,msg);
			return false;
		}
		return true;
	}

	//Lua methods
	static LuaActor* get_actor()
	{
		return Currentactor.local();
	}

	static int send_message(lua_State*	L)
	{
		LuaActor*	actor		= get_actor();
		std::string	target;

		msg_t		msg;
		LuaVM::get_message(L, target, msg);

		AddressSpace::send_message(actor,target,msg);
		return 0;
	}

	static int send_exception(lua_State*	L)
	{
		LuaActor*	actor		= get_actor();
		std::string	target;

		msg_t		msg;
		LuaVM::get_message(L, target, msg);
		if(!msg.has("error"))
		{
			msg.properties["error"]="lua exception";
		}
		AddressSpace::send_exception(actor,target,bad_lua_handling(),msg);
		return 0;
	}

	static int set_property(lua_State*	L)
	{
		LuaActor*	actor		= get_actor();
		std::string	paramName	= lua_tostring(L,1);
		switch(lua_type(L,2))
		{
		case LUA_TNIL:
			{
				actor->properties.erase(paramName);
				break;
			}
		case LUA_TNUMBER:
			{
				actor->properties[paramName] = lua_tonumber(L,2);
				break;
			}
		case LUA_TBOOLEAN:
			{
				actor->properties[paramName] = (lua_toboolean(L,2)!=0);
				break;
			}
		case LUA_TSTRING:
			{
				actor->properties[paramName] = lua_tostring(L,2);
				break;
			}
		default:
			{
				lua_settop(L,0);
				throw bad_lua_property();
			}
		};
		lua_settop(L,0);
		return 0;
	}

	static int get_property(lua_State*	L)
	{
		LuaActor*	actor		= get_actor();
		std::string paramName	= luaL_checkstring(L,1);
		property_map::iterator param = actor->properties.find(paramName);
		if(param!=actor->properties.end())
		{
			switch(param->second.get_type())
			{
			case property_types::NUMBER:
				{
					lua_pushnumber(L,param->second.get_number());
					return 1;
				}
			case property_types::BOOLEAN:
				{
					lua_pushboolean(L,param->second.get_boolean());
					return 1;
				}
			case property_types::STRING:
				{
					lua_pushstring(L,param->second.get_string().c_str());
					return 1;
				}
			case property_types::NIL:
			default:
				{}
			}
		}
		return 0;
	}

	static int spawn_child(lua_State*	L)
	{
		LuaActor*	actor		= get_actor();
		std::string childName	= lua_tostring(L,1);
		LuaActor*	child		= new LuaActor(childName,actor);
		std::string	className;
		if(lua_gettop(L)>1 && lua_type(L,2) == LUA_TSTRING)
		{
			className			= lua_tostring(L,2);
		}
		else
		{
			className			= childName;
		}
		if(!className.empty())
		{
			child->properties["class"]=className;
		}
		AddressSpace::insert<LuaActor>(childName,child);
		AddressSpace::send_message(actor,childName,make_message("init"));
		return 0;
	}

private:
	LuaVM*		vm;
};


static const struct luaL_reg actorlib [] = {
	{"getproperty",		LuaActor::get_property},
	{"setproperty",		LuaActor::set_property},
	{"send",			LuaActor::send_message},
	{"sendexception",	LuaActor::send_exception},
	{"spawn",			LuaActor::spawn_child},
	/*
	Note: full promise support requires LuaCoCo
		and complicates the implementation beyond 
		the scope of this sample. Check the online
		version for a full implementation, at:
			http://code.google.com/p/tbb-actor/
	{"makepromise",		LuaActor::make_promise},
	{"claimpromise",	LuaActor::claim_promise},
	*/
	{NULL, NULL}  /* sentinel */
};

int luaopen_alib (lua_State *L)
{
	luaL_openlib(L, "actorlib", actorlib, 0);
	return 1;
}