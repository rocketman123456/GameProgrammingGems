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
#include<utility>
#include<tbb/task.h>
#include<tbb/concurrent_queue.h>
#include<tbb/atomic.h>
#include<tbb/spin_mutex.h>
#include<exception>
#include<tbb/tbb_exception.h>

namespace act
{

//exception: generic exception that actor failed to handle a message correctly
class bad_message_handling: public tbb::movable_exception<void*>
{
public:
	bad_message_handling()
		: tbb::movable_exception<void*>(NULL)
	{}
	virtual ~bad_message_handling()
	{
		//NOOP
	}
	virtual const char* what() const throw()
	{
		return "actor failed to handle message";
	}
};

//actor: basic actor class that handles all the low level processing logic
template<
	class MESSAGE_TYPE,
	class QUEUE_TYPE = tbb::concurrent_queue<MESSAGE_TYPE>,
	class EXQUEUE_TYPE = tbb::concurrent_queue<std::pair<tbb::tbb_exception*,MESSAGE_TYPE> >
>
class actor
{
public:
	typedef typename actor					actor_t;
	typedef typename MESSAGE_TYPE			msg_t;
	typedef typename QUEUE_TYPE				queue_t;
	typedef typename EXQUEUE_TYPE			exqueue_t;

	actor(actor_t* parent = NULL )
		: _parent(parent)
		, _my_continuation(NULL)
	{
		_is_processing			= false;	//atomic value can't be defaulted
		_processor_task			= NULL;		//atomic value can't be defaulted
		if(_parent)
		{
			//use parent's processing unit
			_processing_root = _parent->_processing_root;
		}
		else
		{
			//become a new root processing unit/task
			_processing_root = new(tbb::task::allocate_root())tbb::empty_task();
			_processing_root->set_ref_count(1);
		}
		assert(_processing_root);
		run();
	}

	bool send_message(actor* target, const msg_t& msg)
	{
		assert(target);
		target->_message_queue.push(msg);
		return target->try_processing(_my_continuation);
	}

	bool send_exception(actor* target, tbb::tbb_exception& e, const msg_t& msg)
	{
		assert(target);
		target->_exception_queue.push(std::make_pair(e.move(),msg));
		return target->try_processing(_my_continuation);
	}

	virtual ~actor()
	{
		cancel();
	}

protected:
	//user-defined behavior implementation hooks
	virtual bool receive(msg_t& message) = 0;

	virtual bool receive_exception(tbb::tbb_exception& exp, msg_t& message) = 0;

	//optional: method called before message handling
	virtual void pre_receive()
	{
		//noop
	}

	//optional: method called after message handling
	virtual void post_receive()
	{
		//noop
	}

	//optional: method called as actor is destroyed, used for cleaning up after failures
	virtual void destroy()
	{
		//noop
	}

	bool invasive_handling(actor* target)
	{
		assert(target);
		return target->process_all_messages();
	}

private:
	void run()
	{
		if(!_parent && _processing_root != NULL )
		{
			tbb::spin_mutex::scoped_lock(_destructor_mutex);
			if(_processing_root != NULL && _processing_root->ref_count()>0)
			{
				_processing_root->wait_for_all();
				_processing_root->set_ref_count(1);
			}
		}
	}

	void cancel()
	{
		if(!_parent && _processing_root != NULL )
		{
			tbb::spin_mutex::scoped_lock(_destructor_mutex);
			if(_processing_root != NULL)
			{
				_processing_root->cancel_group_execution();
				_processing_root = NULL;
			}
		}
	}

	bool try_processing(tbb::empty_task* continuation)
	{
		//NOTE: if NULL set a sentinel as placeholder
		if(_processor_task.compare_and_swap((message_processor_task*)(0x1),NULL)==NULL)
		{
			if(!continuation)
			{
				continuation = new( _processing_root->allocate_continuation() ) tbb::empty_task;
			}
			assert(continuation);
			_my_continuation = continuation;
			
			_processor_task = new(continuation->allocate_child()) message_processor_task(this);
			assert(_processor_task);

			continuation->increment_ref_count();
			continuation->spawn(*_processor_task);
			run();
		}
		return true;
	}

	bool process_all_messages()
	{
		if(_is_processing.compare_and_swap(true, false)==false)
		{
			try
			{
				msg_t msg;
				try
				{
					bool workleft = true;
					while(workleft) //check for work atleast once
					{
						workleft = false;
						std::pair<tbb::tbb_exception*,msg_t> e;
						while(_exception_queue.try_pop(e))
						{
							workleft = true;
							pre_receive();
							msg = e.second;
							if(receive_exception(*e.first,msg))
							{
								e.first->destroy();//clean up handled exception
							}
							else
							{
								e.first->throw_self();
							}
							post_receive();
						}
						while(_message_queue.try_pop(msg))
						{
							workleft = true;
							pre_receive();
							if(!receive(msg) && !receive_exception(bad_message_handling(),msg))
							{
								throw bad_message_handling();
							}
							post_receive();
						}
					};
					//from this point a new task may be allocated for processing
					_is_processing = false;
					_processor_task = NULL;
				}
				catch(tbb::tbb_exception& e)
				{
					post_receive();
					if(_parent) //try to bubble exception to parent
					{
						send_exception(_parent,e,msg);
						return false;
					}
					e.throw_self();
				}
				catch(std::exception& e)
				{
					post_receive();
					tbb::captured_exception ce(typeid(e).name(),e.what());
					if(_parent) //try to bubble exception to parent
					{
						send_exception(_parent,ce,msg);
						return false;
					}
					throw;
				}
			}
			catch(...)
			{
				//root failed to handle exception so terminate root
				return false;
			}
		}
		return true;
	}

	class message_processor_task : public tbb::task
	{
		actor_t*	_actor;
	public:
		message_processor_task(actor_t* a)
			: _actor(a)
		{}

		tbb::task* execute()
		{
			bool ok = _actor->process_all_messages();
			_actor->_my_continuation = NULL;
			if(!ok)
			{
				_actor->destroy();
			};
			return NULL;
		}
	};

	friend class message_processor_task;

private:
	actor_t*								_parent;
	queue_t									_message_queue;
	exqueue_t								_exception_queue;
	tbb::atomic<message_processor_task*>	_processor_task;
	tbb::atomic<bool>						_is_processing;
	tbb::task*								_processing_root;
	tbb::spin_mutex							_destructor_mutex;
	tbb::empty_task*						_my_continuation;
};

} //namespace act
