#pragma once
#include "LockGuardWinAPI.h"
#include <thread>
#include <queue>

//struct Pair {
//	int** matrix;
//	int row;
//};

struct Pair {
	int** matrix;
	int row;
	int start;   // номер первой строки порции
};

class ThreadSafeQueue
{
	CRITICAL_SECTION cs;
	CONDITION_VARIABLE cv;
	std::queue<Pair> queue;
	bool finished{ false };
public:
	ThreadSafeQueue();
	~ThreadSafeQueue();

	void push(Pair elem);
	bool try_pop(Pair& elem);
	void finish();
	bool empty();
};

