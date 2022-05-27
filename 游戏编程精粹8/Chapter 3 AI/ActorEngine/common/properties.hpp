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

#pragma once
#include<assert.h>
#include<string>
#include<map>
#include<exception>
#include<tbb/tbb_exception.h>

namespace property_types
{
enum info
{
	NIL,
	NUMBER,
	STRING,
	BOOLEAN,
};
} //namespace property_types

//exception: requested _type does not match actual property _type (no implicit casting)
class property_type_mismatch: public tbb::movable_exception<void*>
{
public:
	property_type_mismatch()
		:tbb::movable_exception<void*>(NULL)
	{}
	virtual const char* what() const throw()
	{
		return "requested property _type is incorrect";
	}
};

//var_property: variable property that can hold_ref simple Lua types (nil,number,boolean,string)
class	var_property
{
private:
	union pod_data{
		void*				nil;
		double				number;
		bool				boolean;
	};

	pod_data				_data;
	std::string				_string;
	property_types::info	_type;

public:
	var_property()
		: _type(property_types::NIL)
	{
		_data.nil = NULL;
	}

	var_property(const double val)
		: _type(property_types::NUMBER)
	{
		_data.number = val;
	}

	var_property(const int val)
		: _type(property_types::NUMBER)
	{
		_data.number = (double)(val);
	}

	var_property(const long val)
		: _type(property_types::NUMBER)
	{
		_data.number = (double)(val);
	}

	var_property(const std::string& val)
		: _type(property_types::STRING)
	{
		_string = val;
	}

	var_property(const char* val)
		: _type(property_types::STRING)
	{
		_string = val;
	}

	var_property(const bool val)
		: _type(property_types::BOOLEAN)
	{
		_data.boolean = val;
	}

	var_property(const var_property& other)
		: _type(other._type)
		, _string(other._string)
		, _data(other._data)
	{}

	var_property& operator=(const var_property& other)
	{
		_string = other._string;
		_data = other._data;
		_type = other._type;
		return *this;
	}

	~var_property()
	{
		//noop
	}

	property_types::info	get_type()
	{
		return _type;
	}

	double	get_number() const
	{
		if(_type!=property_types::NUMBER)
		{
			throw property_type_mismatch();
		}
		return _data.number;
	}

	const std::string& get_string() const
	{
		if(_type!=property_types::STRING)
		{
			throw property_type_mismatch();
		}
		return _string;
	}

	bool get_boolean() const
	{
		if(_type!=property_types::BOOLEAN)
		{
			throw property_type_mismatch();
		}
		return _data.boolean;
	}

};

std::ostream& operator<<(std::ostream& stream, var_property& self)
{
	switch(self.get_type())
	{
	case property_types::NUMBER:	{stream << self.get_number(); break;}
	case property_types::STRING:	{stream << '"' << self.get_string() << '"'; break;}
	case property_types::BOOLEAN:	{stream << self.get_boolean()?"true":"false"; break;}
	case property_types::NIL:		{stream << "nil"; break;}
	default:						{assert(0); break;}
	}
	return stream;
}

typedef std::map<std::string,var_property>	property_map;
