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
	std::queue<Pair> queue;
public:
	ThreadSafeQueue();
	~ThreadSafeQueue();

	void push(Pair elem);
	bool try_pop(Pair& elem);S
	bool empty();
};