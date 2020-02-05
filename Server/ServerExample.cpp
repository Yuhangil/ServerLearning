#include <stdio.h>
#include <WinSock2.h>
#include <stdlib.h>
#include <locale.h>
#include <stdarg.h>

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
	PLAYER_DATA P_DATA;

} SOCKET_INFO;

void Set_SOCKADDR(SOCKADDR_IN* sock_addr, int family, int port, int addr);
void Set_Data(char* buffer, int flags, int Client_ID, SOCKET_INFO* Sockets[]);

int Get_Header(char* MessageBuffer);
void Get_Data(int buffer[], char* MessageBuffer, size_t size);

void Get_PlayerData(char* MessageBuffer, SOCKET_INFO* Sockets[]);
void Get_PlayerPos(char* MessageBuffer, SOCKET_INFO* Sockets);

void Get_4Bytes(char* Buffer, void* Data, size_t Data_size);
void Get_PlayerPos(char* MessageBuffer, SOCKET_INFO* Sockets[]);

int main(void)
{
	WSADATA wsaData = {};
	WSAEVENT events[WSA_MAXIMUM_WAIT_EVENTS];
	WSANETWORKEVENTS NetworkEvents;


	SOCKET_INFO* SocketInfo;
	SOCKET_INFO* Sockets[WSA_MAXIMUM_WAIT_EVENTS];

	SOCKET hListen = INVALID_SOCKET;
	SOCKET hClient = INVALID_SOCKET;

	SOCKADDR_IN tListenAddr = {};
	SOCKADDR_IN tClientAddr = {};

	int Iaddrlen = 0;
	int iResult = 0;
	int iIndex = 0;

	char MessageBuffer[PACKET_SIZE + 1] = {};	// 클라이언트->서버 버퍼
	char buffer[PACKET_SIZE + 1] = {};		// 서버->클라이언트 버퍼

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);		// 시작
	if (iResult != 0)
	{
		fprintf(stderr, "WSAStartup Failed with Error : %d\n", iResult);
		return 1;
	}
	
	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// IPV4 type tcp protocol
	if (hListen == INVALID_SOCKET)
	{
		fprintf(stderr, "Socket Failed With Error : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	Set_SOCKADDR(&tListenAddr, AF_INET, PORT, INADDR_ANY);

	iResult = bind(hListen, (SOCKADDR*)& tListenAddr, sizeof(tListenAddr));

	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "Bind Failed\n");
		closesocket(hListen);
		WSACleanup();
		return 1;
	}
	iResult = listen(hListen, SOMAXCONN);

	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "Listen Error\n");
		closesocket(hListen);
		WSACleanup();
		return 1;
	}

	SocketInfo = (struct SOCKET_INFO*)calloc(1, sizeof(SOCKET_INFO));
	SocketInfo->socket = hListen;
	Sockets[iIndex] = SocketInfo;

	events[iIndex] = WSACreateEvent();
	if (events[iIndex] == WSA_INVALID_EVENT)
	{
		fprintf(stderr, "Event Create Fail\n");
		closesocket(hListen);
		WSACleanup();
		return 1;
	}

	if (WSAEventSelect(hListen, events[iIndex], FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		fprintf(stderr, "Event Select Fail\n");
		closesocket(hListen);
		WSACleanup();
		return 1;
	}

	iIndex++;	// Index 0 is ServerSocket
	Iaddrlen = sizeof(SOCKADDR_IN);
	
	while (1)
	{



		int iEventIndex = WSAWaitForMultipleEvents(iIndex, events, FALSE, WSA_INFINITE, FALSE);
		if (iEventIndex == WSA_WAIT_FAILED)
		{
			fprintf(stderr, "Event Wait Fail\n");
			break;
		}

		// Get Information of Event Object
		if (WSAEnumNetworkEvents(Sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, events[iEventIndex - WSA_WAIT_EVENT_0], &NetworkEvents) == SOCKET_ERROR)
		{
			fprintf(stderr, "Event Type Error\n");
			break;
		}




		// FD_ACCEPT : Make a socket with Accept() and bind with Event object
		if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
		{
			memset(buffer, 0, sizeof(buffer));

			if (NetworkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
			{
				fprintf(stderr, "Network Accept Fail\n");
				break;
			}

			if (iIndex > WSA_MAXIMUM_WAIT_EVENTS)
			{
				fprintf(stderr, "Connection Full\n");
				break;
			}

			hClient = accept(Sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, ( SOCKADDR*) &tClientAddr, &Iaddrlen);
			if (hClient == INVALID_SOCKET)
			{
				fprintf(stderr, "Client Accept Fail\n");
				continue;
			}

			Sockets[iIndex] = (SOCKET_INFO*)calloc(1, sizeof(SOCKET_INFO));
			Sockets[iIndex]->socket = hClient;
			Sockets[iIndex]->P_DATA.client_id = iIndex;
			
			events[iIndex] = WSACreateEvent();
			if (events[iIndex] == WSA_INVALID_EVENT)
			{
				fprintf(stderr, "Event Create Fail\n");
				closesocket(hClient);
				closesocket(hListen);
				WSACleanup();
				return 1;
			}

			if (WSAEventSelect(hClient, events[iIndex], FD_READ | FD_WRITE) == SOCKET_ERROR)
			{
				fprintf(stderr, "Event Select Fail\n");
				closesocket(hClient);
				closesocket(hListen);
				WSACleanup();
				return 1;
			}
			// 새로 ACCEPT한 Client에게 ID 전달
			Set_Data(buffer, 1, iIndex, Sockets);
			for (int i = 1; i <= iIndex; i++)		// 0번은 server이므로 1부터 시작
			{
				send(Sockets[i]->socket, buffer, sizeof(buffer), 0);
				// player가 참가했음을 전달
			}
			iIndex++;
		}


		if (NetworkEvents.lNetworkEvents & FD_READ)
		{
			int buffer[10];
			memset(buffer, 0, sizeof(buffer));
			memset(MessageBuffer, 0, sizeof(MessageBuffer));
			const wchar_t* ad;
			int receiveBytes = recv(Sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, MessageBuffer, sizeof(MessageBuffer), 0);
			int header = Get_Header(MessageBuffer);
			if ((header & 0xffff0000) != 3644653568)
			{
				printf("우리 패킷이 아님");
			}
			else
			{
				int buffer[10] = {};
				printf("플래그는 : %u\n", header & 0xffff);
				switch (header)
				{
				case 3:		// UpdatePlayerPos
					Get_PlayerPos(MessageBuffer+4, Sockets);
					SendAll(buffer, sockets);
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				}
				ad = (const wchar_t*)(MessageBuffer + 4);
				setlocale(LC_ALL, "");
				if (receiveBytes > 0)
				{
					wprintf(L"TRACE - Receive Message : %s (%d bytes)\n", ad, receiveBytes - 4);
				}
			}
		}


		if (NetworkEvents.lNetworkEvents & FD_WRITE)
		{
			/*int SendBytes = send(Sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, MessageBuffer, sizeof(MessageBuffer), 0);
			if (SendBytes > 0)
			{
				printf("TRACE - Send Message : %s (%d bytes)\n", MessageBuffer, SendBytes);
			}
			*/
		}


		if (NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			int t = Sockets[iEventIndex - WSA_WAIT_EVENT_0]->P_DATA.client_id;
			closesocket(Sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket);

			free((void*)Sockets[iEventIndex - WSA_WAIT_EVENT_0]);

			if (WSACloseEvent(events[iEventIndex]) == TRUE)
			{
				printf("Event Close Success\n");
			}
			else
			{
				fprintf(stderr, "Event Close Fail\n");
			}

			for (int i = iEventIndex; i < iIndex; i++)
			{
				Sockets[i] = Sockets[i + 1];
				events[i] = events[i + 1];
			}
			Set_Data(buffer, 2, t, Sockets);
			for (int i = 0; i < iIndex; i++)
			{
				 send(Sockets[i]->socket, buffer, sizeof(buffer), 0);	// 플레이어가 사라졌음을 모두에게 전달
			}
			iIndex--;
		}

	}

	closesocket(hListen);
	WSACleanup();		// 소켓 종료
	return 0;
}

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
int BTI(char* Buffer)
{
	int t = 0;
	t = t | (unsigned int)(Buffer[0] << 24) | (unsigned int)(Buffer[1] << 16) | (unsigned int)(Buffer[2] << 8) | (unsigned int)(Buffer[3]);
	return t;
}
void Get_4Bytes(char* Buffer, void* Data, size_t Data_size)
{
	memcpy(Data, Buffer, Data_size);
}
int Get_Header(char* MessageBuffer)
{
	int header = 0;
	Get_4Bytes(MessageBuffer, &header, sizeof(header));
	return header;
}

int Set_Header(int flags)
{
	int header = 0;
	char arr[4] = {0xD9, 0x3D, flags& 0xFF, (flags>>8) & 0xFF};
	memcpy(&header, arr, sizeof(arr));
	return header;
}

void Set_Data(char* buffer, int flags, int Client_ID, SOCKET_INFO* Sockets[])
{
	int header = Set_Header(flags);

	memcpy(buffer, &header, sizeof(header));
	memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
	if (flags == 1)		// connection 클라이언트에게 자신의 id번호를 전달
	{
	}
	else if (flags == 2)	// CLIENT_ID 클라이언트의 종료로 인해 다른 클라이언트에게 종료를 알림
	{
	}
	else if (flags == 3)
	{
		memcpy(buffer + 8, &Sockets[Client_ID]->P_DATA.x, sizeof(Sockets[Client_ID]->P_DATA.x));
		memcpy(buffer + 12, &Sockets[Client_ID]->P_DATA.y, sizeof(Sockets[Client_ID]->P_DATA.y));
	}
}

void Get_PlayerData(char* MessageBuffer, SOCKET_INFO* Sockets[])
{
	int client_id = 0;
	Get_4Bytes(MessageBuffer, &client_id, sizeof(client_id));
	Get_4Bytes(MessageBuffer + 4, &(Sockets[client_id]->P_DATA.playername), sizeof(Sockets[client_id]->P_DATA.playername));
}
void Get_PlayerPos(char* MessageBuffer, SOCKET_INFO* Sockets[])
{
	int client_id = 0;
	Get_4Bytes(MessageBuffer, &client_id, sizeof(client_id));
	Get_4Bytes(MessageBuffer + 4, &(Sockets[client_id]->P_DATA.x), sizeof(Sockets[client_id]->P_DATA.x));
	Get_4Bytes(MessageBuffer + 8, &(Sockets[client_id]->P_DATA.y), sizeof(Sockets[client_id]->P_DATA.y));
}
void Get_Connection(char* MessageBuffer, SOCKET_INFO* Sockets[])
{
	int client_id = 0;
	Get_4Bytes(MessageBuffer, &client_id, sizeof(client_id));
	Get_4Bytes(MessageBuffer + 4, &(Sockets[client_id]->P_DATA.playername), sizeof(Sockets[client_id]->P_DATA.playername));
}
/*

패킷 종류
Connection:
   int client_id
   str player_nickname

PlayerPos:
   int client_id
   float x
   float y

Structure Construct:
   int client_id
   int structure_id
   float structure_x
   float structure_y

Disconnection:
   int client_id

접속 흐름:
C1 -> S [Connection(c1)]: 접속 요청

S -> C1 [Connection(c1)]: 접속 허락
S -> C_Other [Connection(c1)]: C1 접속 알림
S -> C1 [Connection(c_others)]: C1에게 다른 클라정보 전달
C_Other: C1 추가
C1: C_Other들에 대한 패킷 받고 추가


종료 흐름:
C1 -> S [Disconnection(c1)]: 종료

S: C1을 클라이언트 리스트에서 삭제
S -> C_All [Disconnection(c1)]: 다른 클라에게 C1의 접속 해제를 알림
C_All: C1을 게임에서 삭제
*/