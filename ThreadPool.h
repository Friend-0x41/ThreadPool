#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include "FixedFreeLinkList.hpp"
#include "WorkThread.h"
#include "TaskList.h"
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>

class _declspec(dllexport) ThreadPool
{
public:

	ThreadPool();

	~ThreadPool()
	{

	}


	template<typename ParamaterStruct>
	void addWork(std::function<void(ParamaterStruct)>&& func, ParamaterStruct ps)
	{
		_lock.lock();
		auto cab = std::make_shared<CallableObj<ParamaterStruct>>(std::bind(&ThreadPool::work_func<ParamaterStruct>, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ThreadPoolParamater<ParamaterStruct> tpp;
		tpp.paramater = ps;
		tpp.func = std::move(func);
		cab->passParamater(std::move(tpp));
		if (!_workThreads.empty())
		{
			startWork(cab);
		}
		else
		{
			_taskList.addTask(cab);
		}
		_lock.unlock();
	}

	void addWork(std::function<void()>&& func);

	void startWork(std::shared_ptr<CallableObjBase> cab);

	size_t thread_used() const noexcept;

private:

	template<typename ParamaterStruct>
	void work_func(std::function<void(ParamaterStruct)>&& func, ParamaterStruct ps, void* node)
	{
		func(ps);
		auto nwt = (Node<WorkThread>*)node;
		nwt->ele.setWorkStatus(false);
		_workThreads.ReleaseFreeNode(nwt);
		checkTaskList();
	}

	void work_func(std::function<void()>&& func, void* node);

	void checkTaskList() noexcept;

private:

	std::mutex _lock;

	const size_t _threadLimit = 16;

	FixedFreeLinklist<WorkThread> _workThreads;

	TaskList _taskList;
};

#endif
