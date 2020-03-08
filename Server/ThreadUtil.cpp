#include "ThreadUtil.h"
#include <process.h>

HANDLE CThreadUtil::hMutex = NULL;

int CThreadUtil::MakeThread(HANDLE* hThread, DWORD* dwThreadID, unsigned(*ThreadFunction)(void*), void* ArgList)
{
	*hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction, ArgList, 0, (unsigned*)dwThreadID);
	if (hThread == 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
	return 0;
}

int CThreadUtil::InitMutex()
{
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL)
		return -1;
	else
		return 0;
}
