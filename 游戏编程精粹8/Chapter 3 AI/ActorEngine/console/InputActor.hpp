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
#include<stdio.h>
#include<limits>
#include<conio.h>

#include"PropertyMessage.hpp"
#include"SimpleActor.hpp"

//Responsability: Check for input from the command-line and pass it in to the system
class InputActor : public SimpleActor
{
	bool			asyncInput;
	std::string		listener;
public:
	InputActor(const std::string& name,SimpleActor* parent = NULL)
		: SimpleActor(name,parent)
		, asyncInput(true)
		, listener("")
	{}

	virtual bool receive(msg_t& msg)
	{
		if( msg.has("key"))
		{
			AddressSpace::send_message(this,listener,msg);
			return true;
		}
		if( msg.has("cmd"))
		{
			AddressSpace::send_message(this,listener,msg);
			return true;
		}
		if( msg.has("tick"))
		{
			checkInput();
			return true;
		}
		if( msg.has("mode"))
		{
			if( msg.properties.find("mode")->second.get_string() == "async" )
			{
				asyncInput = true;
			}
			else
			{
				asyncInput = false;
			}
			return true;
		}
		if( msg.has("register"))
		{
			listener = msg.properties.find("register")->second.get_string();
			return true;
		}
		return true;
	}

	virtual bool receive_exception(tbb::tbb_exception& e, msg_t& msg)
	{
		AddressSpace::send_exception(this,"Logger",e,msg);
		return true;
	}

	void checkInput()
	{
		if(asyncInput) //key input
		{
			if(kbhit())
			{
				send_message(this,make_message("key",getch()));
			}
		}
		else //command-line input
		{
			std::string str;
			std::getline(std::cin,str);
			if(std::cin)
			{
				send_message(this,make_message("cmd",str));
			}
			else
			{
				std::cin.clear();
#ifdef max
# undef max
#endif
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
			}
		}
	}
};
