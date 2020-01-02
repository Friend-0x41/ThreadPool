#ifndef TASKLIST_HPP_
#define TASKLIST_HPP_

#include "CallableObj.h"
#include <memory>
#include <mutex>

struct TaskListNode
{
	~TaskListNode() = default;
	TaskListNode* next{ nullptr };
	std::shared_ptr<CallableObjBase> callableObj;
};

class _declspec(dllexport) TaskList
{
	struct Chunk
	{
		enum
		{
			storage_sz = sizeof(TaskListNode) * 1024
		};
		Chunk* next{ nullptr };
		char mem[storage_sz];
	};

public:

	~TaskList();

	TaskListNode* getFirstTask() noexcept;

	void releaseTask(TaskListNode* node) noexcept;

	bool addTask(std::shared_ptr<CallableObjBase> cab) noexcept;

	bool empty() noexcept;

private:

	bool grow() noexcept;

private:

	TaskListNode* _used_head{ nullptr };
	TaskListNode* _used_last{ nullptr };
	TaskListNode* _free_head{ nullptr };
	Chunk* _chunk_head{ nullptr };

	std::mutex _lock;
};

#endif