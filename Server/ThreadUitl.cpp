#include "ThreadUtil.h"

HANDLE hMutex;

int MakeThread(HANDLE* hThread, DWORD* dwThreadID, unsigned (*ThreadFunction)(void*), void* ArgList)
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
}

int InitMutex()
{
	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL)
		return -1;
	else
		return 0;
}

/* 사용법 예시
int hThread = NULL;
DWORD dwThreadID = NULL;

if(MakeThead(&hThread, &dwThreadID, Function, ArgList*) == -1)
{
	puts("ERROR!");
}
else
{
	Something to do;
}

*/