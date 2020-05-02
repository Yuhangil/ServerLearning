#include "ThreadManager.h"
#include <process.h>

CThreadManager::CThreadManager()
{
	InitMutex();
}

CThreadManager::~CThreadManager()
{
	::CloseHandle(hMutex);
}

int CThreadManager::MakeThread(HANDLE* hThread, DWORD* dwThreadID, unsigned(*ThreadFunction)(void*), void* ArgList)
{
	*hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction, ArgList, 0, (unsigned*)dwThreadID);
	if (hThread == 0)
	{
		return -1;
	}

	//::CloseHandle(hThread);
	return 0;
}

int CThreadManager::InitMutex()
{
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL)
		return -1;
	else
		return 0;
}
