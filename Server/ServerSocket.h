#pragma once
#include <WinSock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32")

#define PORT 8282 // 임의의 포트
#define PACKET_SIZE 1024 // 임의

typedef struct SOCKET_DATA
{
	int header; // 0~7 bit checksum , 8~23bit data length, 24~31 bit data type
	char Message[512];
}DATA;

typedef struct PLAYER_DATA
{
	char playername[31];
	int client_id;
	float x, y;
}PLAYER_DATA;
typedef struct SOCKET_INFO
{
	SOCKET socket;
	DATA message;
	int receiveBytes;
	int sendBytes;
	clock_t last_send;
	PLAYER_DATA P_DATA;

} SOCKET_INFO;


void Send_All(char* buffer, SOCKET_INFO* Sockets[], int iIndex, int Client_ID);
void Set_SOCKADDR(SOCKADDR_IN* sock_addr, int family, int port, int addr);
void TimeOut(SOCKET_INFO* Sockets[]);