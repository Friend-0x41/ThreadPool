#ifndef FIXEDFREELINKLIST_HPP_
#define FIXEDFREELINKLIST_HPP_

#include <new>
#include <mutex>

template<typename T>
struct Node
{
	~Node() = default;

	Node<T>* next{ nullptr };
	T ele;
};

template<typename ElementType>
class FixedFreeLinklist
{
public:

	FixedFreeLinklist(size_t fixedSize)
		:_fixedSize(fixedSize)
	{
		buildChunk();
	}

	~FixedFreeLinklist()
	{
		for (size_t i = 0; i < _fixedSize; ++i)
		{
			((Node<ElementType>*)(_memory + sizeof(Node<ElementType>) * i))->~Node<ElementType>();
		}
		delete[] _memory;
	}

	Node<ElementType>* getFreeNode() noexcept
	{
		_mutex.lock();
		Node<ElementType>* node{ nullptr };
		if (_freeHead)
		{
			node = _freeHead;
			_freeHead = _freeHead->next;
		}
		--_count;
		_mutex.unlock();
		return node;
	}

	bool empty() const noexcept
	{
		return !_freeHead;
	}

	void ReleaseFreeNode(Node<ElementType>* node) noexcept
	{
		_mutex.lock();
		node->next = _freeHead;
		_freeHead = node;
		++_count;
		_mutex.unlock();
	}

	size_t size() const noexcept
	{
		return _count;
	}

	FixedFreeLinklist(const FixedFreeLinklist& other) = delete;
	FixedFreeLinklist(FixedFreeLinklist&& other) = delete;
	FixedFreeLinklist& operator=(const FixedFreeLinklist& other) = delete;
	FixedFreeLinklist& operator=(FixedFreeLinklist&& other) = delete;

private:

	void buildChunk() noexcept
	{
		size_t sz = sizeof(Node<ElementType>) * _fixedSize;
		_memory = new(std::nothrow) char[sz];
		if (!_memory)
		{
			return;
		}
		_freeHead = new(_memory) Node<ElementType>();
		auto node = _freeHead;
		for (size_t i = 1; i < _fixedSize; ++i)
		{
			auto pnew = new(_memory + i * sizeof(Node<ElementType>)) Node<ElementType>();
			node->next = pnew;
			node = pnew;
		}
		_count = _fixedSize;
	}

private:

	Node<ElementType>* _freeHead{ nullptr };

	char* _memory{ nullptr };

	size_t _count;

	const size_t _fixedSize;

	std::mutex _mutex;
};

#endif