#ifndef DEFINE_H_
#define DEFINE_H_

#include <functional>

template<typename T>
struct ThreadPoolParamater
{
	std::function<void(T)> func;
	T paramater;
};


struct ThreadPoolParamaterWithoutParamater
{
	std::function<void()> func;
};
#endif