#include "ServerSocket.h"
#include "ThreadUtil.h"

extern HANDLE hMutex;

void Send_All(char* buffer, SOCKET_INFO* Sockets[], int iIndex, int Client_ID)
{
	int i;
	for (i = 1; i <= iIndex; i++)
	{
		if (i != Client_ID)
		{
			send(Sockets[i]->socket, buffer, sizeof(buffer), 0);
		}
	}
}
void Set_SOCKADDR(SOCKADDR_IN* sock_addr, int family, int port, int addr)
{
	sock_addr->sin_family = family;
	sock_addr->sin_port = htons(port);
	sock_addr->sin_addr.S_un.S_addr = htonl(addr);
}
void TimeOut(SOCKET_INFO* Sockets[], int iIndex)
{
	int i;
	clock_t T = clock();
	WaitForSingleObject(hMutex, INFINITE);
	for (i = 1; i < iIndex; i++)
	{
		if ((T - Sockets[i]->last_send) / CLOCKS_PER_SEC)
		{
			// 소켓 종료 절차 밟기
		}
	}
	ReleaseMutex(hMutex);
}