#include "ThreadPool.h"
#include <iostream>
#include <memory>
#include <functional>
#include <string>

std::mutex lock;

size_t i = 0;

void helloWorld()
{
	lock.lock();
	std::cout << i++ << "\thello world" << std::endl;
	lock.unlock();
}

int main()
{
	ThreadPool tp;
	for (int i = 0; i < 100000; ++i)
	{
		tp.addWork(std::move(helloWorld));
	}
	std::cin.get();

	return 0;
}