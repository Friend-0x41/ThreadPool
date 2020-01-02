#include "WorkThread.h"
#include <thread>
#include <iostream>

WorkThread::WorkThread()
{
	_stop.store(false);
	ThreadDetachGuard tg(std::thread{ [this]() {
		work();
	} });
}

WorkThread::~WorkThread()
{
	_stop.store(true);
}

bool WorkThread::addWork(std::shared_ptr<CallableObjBase> cab)
{
	//_lock.lock();
	if (_isWorking)
	{
		//_lock.unlock();
		return false;
	}	
	_cab = cab;
	//_lock.unlock();
	return true;
}

bool WorkThread::isWorking() const noexcept
{
	return _isWorking.load();
}

void WorkThread::setWorkStatus(bool isWorking)
{
	_isWorking.store(isWorking);
}

void WorkThread::work()
{
	while (true)
	{
		if (_cab)
		{
			setWorkStatus(true);
			auto cab = _cab;
			_cab.reset();
			cab->operator()();
		}
		while (true)
		{
			if (_stop.load())
			{
				return;
			}
			if (!_cab)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
			else
			{
				break;
			}
		}
	}
}

WorkThread::ThreadDetachGuard::ThreadDetachGuard(std::thread&& t)
	:_t(std::move(t))
{
	
}

WorkThread::ThreadDetachGuard::~ThreadDetachGuard()
{
	if (_t.joinable())
	{
		_t.detach();
	}
	else
	{
		printf("no thread\n");
	}
}
