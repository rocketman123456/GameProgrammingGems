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

#include"tbb/tbbmalloc_proxy.h"

#include"PropertyMessage.hpp"
#include"SimpleActor.hpp"
#include"LogActor.hpp"
#include"InputActor.hpp"
#include"ClockActor.hpp"
#include"LuaActor.hpp"

#include<tbb/task_scheduler_init.h>

int main(int argc, char* argv[])
{
	int number_of_threads = tbb::task_scheduler_init::automatic;
	if(argc==2)
	{
		std::stringstream arg(argv[1]);
		arg >> number_of_threads;
	}
	if(number_of_threads>0 || number_of_threads == tbb::task_scheduler_init::automatic)
	{
		tbb::task_scheduler_init scheduler(number_of_threads);
		try
		{
			LuaVM::mainscript	= "./scripts/main.lua";

			LuaActor*	root	= SimpleActor::AddressSpace::insert<LuaActor>("Root");
			ClockActor*	clock	= SimpleActor::AddressSpace::insert<ClockActor>("Clock");
			LogActor*	logger	= SimpleActor::AddressSpace::insert<LogActor>("Logger");
			InputActor*	input	= SimpleActor::AddressSpace::insert<InputActor>("Input");

			//start engine
			SimpleActor::AddressSpace::send_message(root,"Root",make_message("init"));

			SimpleActor::msg_t			tick = make_message("tick");
			tbb::tick_count::interval_t	timerstep( 1.0/60.0 );

			while(SimpleActor::AddressSpace::exists("Root"))
			{
				//tick the clock actor,
				//all the actual work is being handled 
				//by the scheduler behind the scenes
				SimpleActor::AddressSpace::send_message("Clock",tick);
				tbb::this_tbb_thread::sleep( timerstep );
			};
		}
		catch(tbb::tbb_exception& e)
		{
			std::cout<< e.what() <<std::endl;
		}

		//terminate engine
		scheduler.terminate();

		//cleanup all actors
		SimpleActor::AddressSpace::clear();

		//cleanup the Lua VMs
		{
			LuaVMPool_t::iterator iter = LuaVMPool.begin();
			LuaVMPool_t::iterator end = LuaVMPool.end();
			for(;iter!=end;++iter)
			{
				iter->close();
			}
		}

		return 0;
	}
	return 100;
}
