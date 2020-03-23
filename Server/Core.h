#pragma once
#include "Server.h"
#include "World.h"

class CCore
{
public:
	int index;

public:
	WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET_INFO* sockets[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET hListen;
	int eventIndex;

private:
	int addrlen;
	CWorld* world;

public:
	CCore();
	~CCore();

public:
	bool Init();
	int Listen();
	void Send_All(char* buffer, size_t buffersize, int Client_ID);
	bool Close();

private:
	void SetSOCKADDR(SOCKADDR_IN* sock_addr, int family, int port, int addr);
	void TimeOut();
	void CloseSocket(int CloseSocketIndex);
};

