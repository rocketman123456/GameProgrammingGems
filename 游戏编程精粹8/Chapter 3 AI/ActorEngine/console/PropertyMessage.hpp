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

#include<actor.hpp>
#include<properties.hpp>

struct PropertyMessage
{
	typedef property_map::iterator			iterator;
	typedef property_map::const_iterator	const_iterator;

	property_map							properties;

	PropertyMessage()
	{}

	PropertyMessage(const PropertyMessage& other)
		: properties(other.properties)
	{}

	PropertyMessage& operator=(const PropertyMessage& other)
	{
		properties = other.properties;
		return *this;
	}

	bool has(const std::string& var)
	{
		return properties.find(var)!=properties.end();
	}

};

PropertyMessage make_message(const std::string& name0)
{
	PropertyMessage msg;
	msg.properties[name0]=name0;
	return msg;
}

template<typename V0>
PropertyMessage make_message(const std::string& name0, const V0& var0)
{
	PropertyMessage msg;
	msg.properties[name0]=var0;
	return msg;
}

template<typename V0, typename V1>
PropertyMessage make_message(const std::string& name0, const V0& var0, const std::string& name1, const V1& var1)
{
	PropertyMessage msg;
	msg.properties[name0]=var0;
	msg.properties[name1]=var1;
	return msg;
}

template<typename V0, typename V1, typename V2>
PropertyMessage make_message(const std::string& name0, const V0& var0, const std::string& name1, const V1& var1, const std::string& name2, const V2& var2)
{
	PropertyMessage msg;
	msg.properties[name0]=var0;
	msg.properties[name1]=var1;
	msg.properties[name2]=var2;
	return msg;
}

template<typename V0, typename V1, typename V2, typename V3>
PropertyMessage make_message(const std::string& name0, const V0& var0, const std::string& name1, const V1& var1, const std::string& name2, const V2& var2, const std::string& name3, const V3& var3)
{
	PropertyMessage msg;
	msg.properties[name0]=var0;
	msg.properties[name1]=var1;
	msg.properties[name2]=var2;
	msg.properties[name3]=var3;
	return msg;
}
