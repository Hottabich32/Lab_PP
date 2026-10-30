#include "LockGuardWinAPI.h"

LockGuardWinAPI::LockGuardWinAPI(CRITICAL_SECTION& cs) {
	section = &cs;
	EnterCriticalSection(section);
}

LockGuardWinAPI::~LockGuardWinAPI() {
	LeaveCriticalSection(section);
}S