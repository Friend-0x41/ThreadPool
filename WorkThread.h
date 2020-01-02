#ifndef WORK_THREAD_H_
#define WORK_THREAD_H_

#include "CallableObj.h"
#include <thread>
#include <atomic>

class CallableObjBase;

class _declspec(dllexport) WorkThread
{
	class ThreadDetachGuard
	{
	public:
		ThreadDetachGuard(std::thread&& t);

		~ThreadDetachGuard();

	private:

		std::thread _t;
	};
public:

	WorkThread();

	~WorkThread();

	bool addWork(std::shared_ptr<CallableObjBase> cab);

	bool isWorking() const noexcept;

private:

	void setWorkStatus(bool isWorking);

	void work();

private:

	std::atomic_bool _isWorking = false;

	std::atomic_bool _stop;

	std::shared_ptr<CallableObjBase> _cab;

	friend class ThreadPool;
};

#endif