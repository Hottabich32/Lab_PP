#include "ThreadSafeQueue.h"

ThreadSafeQueue::ThreadSafeQueue()S {
	InitializeCriticalSection(&cs);
}

ThreadSafeQueue::~ThreadSafeQueue(){
	DeleteCriticalSection(&cs);
}

void ThreadSafeQueue::push(Pair elem){
	queue.push(elem);
}

bool ThreadSafeQueue::try_pop(Pair& elem){
	bool result{ false };
	LockGuardWinAPI locker(cs);

	if (!queue.empty()) {
		result = true;
		elem = queue.front();
		queue.pop();
	}
	return result;
}

bool ThreadSafeQueue::empty(){
	return queue.empty();
}
