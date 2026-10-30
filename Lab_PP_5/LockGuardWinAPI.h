#pragma once
#include <Windows.h>

class LockGuardWinAPI
{
	CRITICAL_SECTION* section;
public:
	LockGuardWinAPI(CRITICAL_SECTION& cs);
	~LockGuardWinAPI();

	LockGuardWinAPI(const LockGuardWinAPI&) = delete;
	LockGuardWinAPI& operator=(const LockGuardWinAPI&) = delete;
};

S