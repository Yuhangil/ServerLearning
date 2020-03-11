#pragma once
#include "Server.h"
#include "World.h"

class CCore
{
public:
	int m_iIndex;

public:
	WSAEVENT m_events[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET_INFO* m_sockets[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET m_hListen;
	int iEventIndex;

private:
	int m_iaddrlen;
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

