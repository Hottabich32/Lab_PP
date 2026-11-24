#pragma once
#include "LockGuardWinAPI.h"
#include <thread>
#include <queue>

struct Pair {
	int** matrix;
	int row;
};

class ThreadSafeQueue
{
	CRITICAL_SECTION cs;
	CONDITION_VARIABLE cvNotEmpty;
	CONDITION_VARIABLE cvNotFull;
	std::queue<Pair> queue;
	size_t capacity;
	bool closed;
public:
	explicit ThreadSafeQueue(size_t cap = 3);
	~ThreadSafeQueue();

	void push(Pair elem);
	bool pop(Pair& elem);
	bool try_pop(Pair& elem);
	bool empty();
	void close();
};