#pragma once

#include "Server.h"

class CDataUtil
{

public:
	static void Get4Bytes(char* Buffer, void* Data, size_t Data_size);

	static int GetHeader(char* MessageBuffer);

	static void GetPlayerData(char* MessageBuffer, SOCKET_INFO* Sockets[]);
	static void GetPlayerPos(char* MessageBuffer, SOCKET_INFO* Sockets[], int Client_ID);
	static void GetConnection(char* MessageBuffer, SOCKET_INFO* Sockets[]);

	static void SetData(char* buffer, size_t buffersize, int flags, int Client_ID, SOCKET_INFO* Sockets[]);
	static int SetHeader(int iflags);
};

