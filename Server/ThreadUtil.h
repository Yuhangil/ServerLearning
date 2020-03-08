#pragma once
#include "Server.h"

class CThreadUtil
{
public :
	static HANDLE hMutex;

public:
	static int MakeThread(HANDLE* hThread, DWORD* dwThreadID, unsigned (*ThreadFunction)(void*), void* ArgList);
	static int InitMutex();
};



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