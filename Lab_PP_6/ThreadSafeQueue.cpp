#include "ThreadSafeQueue.h"

ThreadSafeQueue::ThreadSafeQueue(size_t cap) : capacity(cap), closed(false) {
	InitializeCriticalSection(&cs);
	InitializeConditionVariable(&cvNotEmpty);
	InitializeConditionVariable(&cvNotFull);
}

ThreadSafeQueue::~ThreadSafeQueue() {
	DeleteCriticalSection(&cs);
}

void ThreadSafeQueue::push(Pair elem) {
	LockGuardWinAPI locker(cs);
	while (queue.size() >= capacity && !closed)
		SleepConditionVariableCS(&cvNotFull, &cs, INFINITE);

	if (!closed) {
		queue.push(elem);
		WakeConditionVariable(&cvNotEmpty);
	}
}

bool ThreadSafeQueue::try_pop(Pair& elem) {
	bool result{ false };
	LockGuardWinAPI locker(cs);

	while (queue.empty() && !closed)
		SleepConditionVariableCS(&cvNotEmpty, &cs, INFINITE);

	if (!queue.empty()) {
		result = true;
		elem = queue.front();
		queue.pop();
		WakeConditionVariable(&cvNotFull);
	}
	return result;
}

bool ThreadSafeQueue::empty() {
	return queue.empty();
}

void ThreadSafeQueue::close() {
	LockGuardWinAPI locker(cs);
	closed = true;
	WakeAllConditionVariable(&cvNotEmpty);
	WakeAllConditionVariable(&cvNotFull);

}


