#include "TaskList.h"
#include <new>

TaskList::~TaskList()
{
	{
		auto current = _free_head;
		while (current)
		{
			current->~TaskListNode();
			current = current->next;
		}
	}
	{
		auto current = _used_head;
		while (current)
		{
			current->~TaskListNode();
			current = current->next;
		}
	}
	{
		auto current = _chunk_head;
		while (current)
		{
			auto tmp = current;
			current = current->next;
			delete tmp;
		}
	}
}

bool TaskList::grow() noexcept
{
	auto ptr = new(std::nothrow) Chunk;
	if (!ptr)
	{
		return false;
	}
	size_t sz = Chunk::storage_sz / sizeof(TaskListNode);
	_free_head = reinterpret_cast<TaskListNode*>(ptr->mem);
	auto current = _free_head;
	for (size_t i = 0; i < sz; ++i)
	{
		auto node = new(ptr->mem + i * sizeof(TaskListNode)) TaskListNode;
		current->next = node;
		current = node;
	}
	ptr->next = _chunk_head;
	_chunk_head = ptr;
	return true;
}

TaskListNode* TaskList::getFirstTask() noexcept
{
	_lock.lock();
	auto node = _used_head;
	if (_used_head)
	{
		_used_head = _used_head->next;
	}
	else
	{
		_used_last = nullptr;
	}
	_lock.unlock();
	return node;
}

bool TaskList::addTask(std::shared_ptr<CallableObjBase> cab) noexcept
{
	_lock.lock();
	if (!_free_head)
	{
		if (!grow())
		{
			_lock.unlock();
			return false;
		}
	}
	auto node = _free_head;
	_free_head = _free_head->next;
	node->next = nullptr;
	if (!_used_head)
	{
		_used_head = _used_last = node;
	}
	else
	{
		_used_last->next = node;
		_used_last = node;
	}
	node->callableObj = cab;
	_lock.unlock();
	return true;
}

bool TaskList::empty() noexcept
{
	return !_used_head;
}


void TaskList::releaseTask(TaskListNode* node) noexcept
{
	_lock.lock();
	node->next = _free_head;
	_free_head = node;
	_lock.unlock();
}