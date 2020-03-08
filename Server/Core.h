#pragma once
#include "Server.h"

class CCore
{
public:
	int m_iIndex;

private:
	int m_iaddrlen;

public:
	WSAEVENT m_events[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET_INFO* m_sockets[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET m_hListen;


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
};

