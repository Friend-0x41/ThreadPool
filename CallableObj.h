#ifndef CALLABLE_OBJ_
#define CALLABLE_OBJ_

#include "defines.h"
#include "FixedFreeLinkList.hpp"
#include <cassert>
#include <functional>
#include <memory>

class WorkThread;

class CallableObjBase
{
public:

	virtual void operator()(void) = 0;

	Node<WorkThread>* node{ nullptr };
};

template<typename ParamaterStruct>
class CallableObj : public CallableObjBase
{
public:

	CallableObj(std::function<void(std::function<void(ParamaterStruct)>&&, ParamaterStruct, void *)>&& func)
		:_func(std::move(func))
	{

	}

	virtual void operator()(void) override
	{
		assert(node);
		_func(std::move(_tpp.func), _tpp.paramater, node);
	}

	void passParamater(ThreadPoolParamater<ParamaterStruct>&& tpp) noexcept
	{
		_tpp = std::move(tpp);
	}

	ThreadPoolParamater<ParamaterStruct>& getParamaterRefenrence()
	{
		return _tpp;
	}

private:

	std::function<void(std::function<void(ParamaterStruct)>&&, ParamaterStruct, void*)> _func;

	ThreadPoolParamater<ParamaterStruct> _tpp;
};

class CallableObjNoParamater : public CallableObjBase
{
public:

	CallableObjNoParamater(std::function<void(std::function<void()>&&, void*)>&& func)
		:_func(std::move(func))
	{

	}

	virtual void operator()(void) override
	{
		assert(node);
		_func(std::move(_tpp.func), node);
	}

	void passParamater(ThreadPoolParamaterWithoutParamater&& tpp) noexcept
	{
		_tpp = std::move(tpp);
	}

	ThreadPoolParamaterWithoutParamater& getParamaterRefenrence()
	{
		return _tpp;
	}

private:

	std::function<void(std::function<void()>&&, void*)> _func;

	ThreadPoolParamaterWithoutParamater _tpp;
};

#endif