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
#include<string>
#include<tbb/concurrent_hash_map.h>

#include<actor.hpp>
#include<properties.hpp>

#include"PropertyMessage.hpp"
/////////////////////////////////////////////////////////////////////////////
//base actor class used in the application, has actor name
class SimpleActor : public act::actor<PropertyMessage>
{
	const std::string	_name;
public:
	SimpleActor(const std::string& name, actor_t* parent = NULL)
		: actor_t(parent)
		, _name(name)
	{}

	const std::string&	name() const
	{
		return _name;
	}

	virtual bool receive_exception(tbb::tbb_exception& e, msg_t& msg)
	{
		AddressSpace::send_exception(this,"Logger",e,msg);
		return true;
	}

	//AddressSpace: An actor address space, in this example for simplicity this is enough,
	//but in practice it might be convinient to have several address spaces
	//with specialized organizations,
	class AddressSpace
	{
		typedef tbb::concurrent_hash_map<std::string,SimpleActor*>	address_space_t;
		static  address_space_t										address_space;

	public:
		static bool	exists(const std::string& name)
		{
			address_space_t::accessor address;
			return address_space.find(address,name);
		}

		template<class A>
		static A*	insert(const std::string& name)
		{
			A* actor = new A(name);
			return insert(name,actor);
		}

		template<class A>
		static A*	insert(const std::string& name, A* actor)
		{
			address_space_t::accessor address;
			if(!address_space.find(address,name))
			{
				address_space.insert(address,name);
				address->second = static_cast<SimpleActor*>(actor);
			}
			else
			{
				delete actor; //delete superfluous actor
			}
			return static_cast<A*>(address->second);//return actual actor
		}

		static bool	send_message(SimpleActor* origin,const std::string&  target,SimpleActor::msg_t& msg)
		{
			address_space_t::const_accessor address;
			if(address_space.find(address,target))
			{
				msg.properties["sender"]=origin->name();
				return origin->send_message(address->second,msg);
			}
			return false;
		}

		static bool	send_message(const std::string&  target,SimpleActor::msg_t& msg)
		{
			address_space_t::const_accessor address;
			if(address_space.find(address,target))
			{
				msg.properties["sender"]=address->first;
				return address->second->send_message(address->second,msg);
			}
			return false;
		}

		static bool	send_exception(SimpleActor* origin,const std::string&  target,tbb::tbb_exception& e, SimpleActor::msg_t& msg)
		{
			address_space_t::const_accessor address;
			if(address_space.find(address,target))
			{
				msg.properties["sender"]=origin->name();
				return origin->send_exception(address->second,e,msg);
			}
			return false;
		}

		static bool	remove(SimpleActor* origin)
		{
			address_space_t::accessor address;
			if(address_space.find(address,origin->name()))
			{
				address_space.erase(address);
				return true;
			}
			return false;
		}

		static void clear()
		{
			address_space_t::iterator iter = address_space.begin();
			address_space_t::iterator end = address_space.end();
			while(iter!=end)
			{
				address_space_t::const_accessor address;
				if(address_space.find(address,iter->first))
				{
					delete iter->second;
					address_space.erase(address);
				}
				iter = address_space.begin();
			}
		}

		static bool	do_invasive_handling(SimpleActor* origin,const std::string&  target)
		{
			address_space_t::const_accessor address;
			if(address_space.find(address,target))
			{
				if(!origin->invasive_handling(address->second))
				{
					//actor died so clean it up
					SimpleActor* zombie = address->second;
					address_space.erase(address);
					zombie->destroy();
					return false;
				};
				return true;
			}
			return false;
		}
	};

protected:
	void	destroy()
	{
		AddressSpace::remove(this);
		delete this;
	}
};
//global actor space
SimpleActor::AddressSpace::address_space_t SimpleActor::AddressSpace::address_space;
