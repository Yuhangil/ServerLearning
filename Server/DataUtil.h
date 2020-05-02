#pragma once

#include "Server.h"

class CDataUtil
{

public:
	static void GetBytes(char* Buffer, void* Data, size_t Data_size);

	static int GetHeader(char* MessageBuffer);

	//static void SetData(char* buffer, size_t buffersize, int flags, int Client_ID, SOCKET_INFO* Sockets[]);
	static int SetHeader(int iflags);
	static int ContactHeader(char* buffer, int clientID, int iflags);
	static void DumpPacket(char* buffer, int size);
};

