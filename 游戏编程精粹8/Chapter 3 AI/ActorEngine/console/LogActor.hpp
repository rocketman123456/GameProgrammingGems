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
#include<iostream>
#include<sstream>

#include"PropertyMessage.hpp"
#include"SimpleActor.hpp"

class LogActor : public SimpleActor
{
public:
	LogActor(const std::string& name, SimpleActor* parent = NULL)
		: SimpleActor(name,parent)
	{}

	virtual bool receive(msg_t& msg)
	{
		//header
		if(msg.has("channel"))
		{
			std::cout << "log - [" <<  msg.properties.find("channel")->second.get_string() << "] ";
		}
		else
		{
			std::cout << "log - [actor:" << msg.properties["sender"] << "] ";
		}
		//payload
		if( msg.has("log"))
		{
			std::cout << msg.properties["log"].get_string() << std::endl;
		}
		else
		{
			//Generic Message Dumper
			std::cout << "message:" << std::endl;
			msg_t::iterator iter = msg.properties.begin();
			msg_t::iterator end = msg.properties.end();
			if(iter!=end)
			{
				for(;iter!=end;++iter)
				{
					if(iter->first!="channel")
					{
						std::cout << "\t" << iter->first << " = " << iter->second << std::endl;
					}
				}
			}
			else
			{
				std::cout << "\t[empty]" << std::endl;
			}
		}
		return true;
	}

	virtual bool receive_exception(tbb::tbb_exception& e, msg_t& msg)
	{
		//log exception
		std::cout << "exception - [actor:" << msg.properties["sender"] << "] " << e.what() << std::endl;
		receive(msg);
		return true;
	}
};