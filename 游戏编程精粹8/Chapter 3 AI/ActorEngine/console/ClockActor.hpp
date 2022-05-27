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
#include<string>

#include<tbb/tbb_thread.h>
#include<tbb/tick_count.h>

#include"PropertyMessage.hpp"
#include"SimpleActor.hpp"

class ClockActor : public SimpleActor
{
	struct timer_t
	{
		bool	cycle;
		double	frequency;
		double	next;
	};

	typedef std::map<std::string,timer_t>			timermap_t;
	timermap_t										timers;
	msg_t											tickmsg;
	tbb::tick_count									start;
	bool											active;

public:
	ClockActor(const std::string& name, SimpleActor* parent = NULL)
		: SimpleActor(name,parent)
		, tickmsg(make_message("tick"))
		, start(tbb::tick_count::now())
		, active(true)
	{}

	virtual bool receive(msg_t& msg)
	{
		if( msg.has("register"))
		{
			timer_t t;
			t.frequency		=	msg.properties.find("frequency")->second.get_number();
			if( msg.has("cycle"))
			{
				t.cycle		=	msg.properties.find("cycle")->second.get_boolean();
			}
			else
			{
				t.cycle		=	false;
			}
			t.next			=	0;
			timers[msg.properties.find("register")->second.get_string()] = t;
			return true;
		}
		if( msg.has("unregister"))
		{
			timers.erase(msg.properties.find("unregister")->second.get_string());
			return true;
		}
		if( msg.has("tick"))
		{
			double					now		=	tbb::tick_count::interval_t(tbb::tick_count::now()-start).seconds();
			timermap_t::iterator	iter	=	timers.begin();
			timermap_t::iterator	end		=	timers.end();
			while(iter!=end)
			{
				timer_t& t	=	iter->second;
				if(t.next==0)
				{
					t.next	=	now + t.frequency;
				}
				else if(t.next<now)
				{
					AddressSpace::send_message(this,iter->first,tickmsg);
					if(t.cycle)
					{
						t.next	=	now + t.frequency;
					}
					else
					{
						timermap_t::iterator del = iter++;
						timers.erase(del);
						continue;
					}
				}
				++iter;
			}
			return true;
		}
		return false;
	}

	virtual bool receive_exception(tbb::tbb_exception& e, msg_t& msg)
	{
		AddressSpace::send_exception(this,"Logger",e,msg);
		return true;
	}

};