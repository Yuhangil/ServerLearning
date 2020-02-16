#pragma once
#include <process.h>
#include <Windows.h>

int MakeThread(HANDLE* hThread, DWORD* dwThreadID, unsigned (*ThreadFunction)(void*), void* ArgList);
int InitMutex();