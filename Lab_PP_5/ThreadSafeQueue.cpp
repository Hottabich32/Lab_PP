#include "ThreadSafeQueue.h"

ThreadSafeQueue::ThreadSafeQueue() : finished(false) {
	InitializeCriticalSection(&cs);
	InitializeConditionVariable(&cv);
}

ThreadSafeQueue::~ThreadSafeQueue(){
	DeleteCriticalSection(&cs);
}

void ThreadSafeQueue::push(Pair elem){
	queue.push(elem);
	WakeConditionVariable(&cv);
}

bool ThreadSafeQueue::try_pop(Pair& elem){
	LockGuardWinAPI locker(cs);

	while(queue.empty() && !finished)
		SleepConditionVariableCS(&cv, &cs, INFINITE);

	if (queue.empty())
		return false;

	elem = queue.front();
	queue.pop();
	return true;
}

void ThreadSafeQueue::finish(){
	finished = true;
	WakeAllConditionVariable(&cv);
}

bool ThreadSafeQueue::empty(){
	return queue.empty();
}
