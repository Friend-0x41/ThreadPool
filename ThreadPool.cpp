#include "defines.h"
#include "ThreadPool.h"
#include "CallableObj.h"

ThreadPool::ThreadPool()
	:_workThreads{16}
{

}

void ThreadPool::addWork(std::function<void()>&& func)
{
	_lock.lock();
	auto cab = std::make_shared<CallableObjNoParamater>(std::move(std::bind(&ThreadPool::work_func, this, std::placeholders::_1, std::placeholders::_2)));
	ThreadPoolParamaterWithoutParamater tpp;
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

void ThreadPool::work_func(std::function<void()>&& func, void* node)
{
	func();
	auto nwt = (Node<WorkThread>*)node;
	nwt->ele.setWorkStatus(false);
	_workThreads.ReleaseFreeNode(nwt);
	checkTaskList();
}

void ThreadPool::checkTaskList() noexcept
{
	_lock.lock();
	if (!_taskList.empty() && !_workThreads.empty())
	{
		auto node = _taskList.getFirstTask();
		startWork(node->callableObj);
		_taskList.releaseTask(node);
	}
	else
	{
		printf("no task\n");
	}
	_lock.unlock();
}

void ThreadPool::startWork(std::shared_ptr<CallableObjBase> cab)
{
	auto node = _workThreads.getFreeNode();
	cab->node = node;
	node->ele.addWork(cab);
}

size_t ThreadPool::thread_used() const noexcept
{
	return _threadLimit - _workThreads.size();
}