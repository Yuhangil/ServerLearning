#pragma once
#include "Server.h"
#include "Player.h"
#include "DataUtil.h"
#include "ThreadManager.h"
#include "World.h"

class CCore
{
public:
	int index;

public:
	WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET_INFO* sockets[WSA_MAXIMUM_WAIT_EVENTS];
	CPlayer* players[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET hListen;
	int eventIndex;

private:
	int addrlen;
	std::vector<CWorld*> worlds;
	CThreadManager* threadManager;

public:
	bool Init();
	int Listen();
	void Send_All(char* buffer, size_t buffersize, int Client_ID);
	bool Close();

private:
	void SetSOCKADDR(SOCKADDR_IN* sock_addr, int family, int port, int addr);
	void TimeOut();
	void CloseSocket(int CloseSocketIndex);

private:
	bool AddWorld(int seed);

public:
	static unsigned int WorldUpdate(LPVOID p);

	DECLARE_SINGLE(CCore)

};