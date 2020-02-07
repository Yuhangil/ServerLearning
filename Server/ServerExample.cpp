#include <stdio.h>
#include <WinSock2.h>
#include <stdlib.h>
#include <locale.h>
#include <stdarg.h>

#pragma comment(lib, "ws2_32")

#define PORT 8282 // ������ ��Ʈ
#define PACKET_SIZE 1024 // ����

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

void Send_All(char* buffer, SOCKET_INFO* Sockets[], int iIndex, int Client_ID);

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

	char MessageBuffer[PACKET_SIZE + 1] = {};	// Ŭ���̾�Ʈ->���� ����
	char buffer[PACKET_SIZE + 1] = {};		// ����->Ŭ���̾�Ʈ ����

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);		// ����
	if (iResult != 0)
	{
		fprintf(stderr, "WSAStartup Failed with Error : %d\n", iResult);
		return 1;
	}
	printf("WSAStartup() OK!\n");

	

	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// IPV4 type tcp protocol
	if (hListen == INVALID_SOCKET)
	{
		fprintf(stderr, "Socket Failed With Error : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	printf("Socket() OK!\n");


	Set_SOCKADDR(&tListenAddr, AF_INET, PORT, INADDR_ANY);


	iResult = bind(hListen, (SOCKADDR*)& tListenAddr, sizeof(tListenAddr));


	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "Bind Failed\n");
		closesocket(hListen);
		WSACleanup();
		return 1;
	}
	printf("Socket Bind OK!\n");
	
	iResult = listen(hListen, SOMAXCONN);


	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "Listen Error\n");
		closesocket(hListen);
		WSACleanup();
		return 1;
	}
	printf("Socket Listen OK!\n");



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
	printf("WSACreateEvent() OK!\n");



	if (WSAEventSelect(hListen, events[iIndex], FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		fprintf(stderr, "Event Select Fail\n");
		closesocket(hListen);
		WSACleanup();
		return 1;
	}
	printf("WSAEventSelect OK!\n");

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
		else
		{
			printf("WSAWaitForMultipleEvents() OK!\n");
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

			if (WSAEventSelect(hClient, events[iIndex], FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
			{
				fprintf(stderr, "Event Select Fail\n");
				closesocket(hClient);
				closesocket(hListen);
				WSACleanup();
				return 1;
			}
			printf("%d�� Ŭ�� ����\n", iIndex);
			// ���� ACCEPT�� Client���� ID ����
			Set_Data(buffer, 1, iIndex, Sockets);
			for (int i = 1; i <= iIndex; i++)		// 0���� server�̹Ƿ� 1���� ����
			{
				send(Sockets[i]->socket, buffer, sizeof(buffer), 0);
				// player�� ���������� ����
			}
			iIndex++;
		}


		if (NetworkEvents.lNetworkEvents & FD_READ)
		{
			memset(buffer, 0, sizeof(buffer));
			memset(MessageBuffer, 0, sizeof(MessageBuffer));
			const wchar_t* ad;
			int receiveBytes = recv(Sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, MessageBuffer, sizeof(MessageBuffer), 0);
			int header = Get_Header(MessageBuffer);
			if ((header & 0xFFFF0000) != 0xD93D0000)
			{
				printf("�츮 ��Ŷ�� �ƴ�");
			}
			else
			{
				int Client_ID;
				Get_4Bytes(MessageBuffer, &Client_ID, sizeof(Client_ID));
				printf("�÷��״� : %u\n", header & 0xFFFF);
				switch (header & 0xFFFF)
				{
				case 3:		// UpdatePlayerPos
					Get_PlayerPos(MessageBuffer+4, Sockets);
					Send_All(buffer, Sockets, iIndex, Client_ID);
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
				/*
				ad = (const wchar_t*)(MessageBuffer + 4);
				setlocale(LC_ALL, "");
				if (receiveBytes > 0)
				{
					wprintf(L"TRACE - Receive Message : %s (%d bytes)\n", ad, receiveBytes - 4);
				}
				*/
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
			printf("%d�� Ŭ�� ����\n", t);
			Set_Data(buffer, 2, t, Sockets);
			iIndex--;
			for (int i = 0; i < iIndex; i++)
			{
				 send(Sockets[i]->socket, buffer, sizeof(buffer), 0);	// �÷��̾ ��������� ��ο��� ����
			}
			
		}

	}

	closesocket(hListen);
	WSACleanup();		// ���� ����
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
	char buff[4] = {};
	int header = 0;
	int t = 0;
	Get_4Bytes(MessageBuffer, buff, sizeof(buff));
	t = buff[0];
	buff[0] = buff[3];
	buff[3] = t;
	t = buff[1];
	buff[1] = buff[2];
	buff[2] = t;
	Get_4Bytes(buff, &header, sizeof(header));
	printf("%x\n", header);
	return header;
}

int Set_Header(int flags)
{
	int header = 0;
	char arr[4] = { flags& 0xFF , (flags >> 8 ) & 0xFF, 0x3D, 0xD9 };
	memcpy(&header, arr, sizeof(arr));
	return header;
}

void Set_Data(char* buffer, int flags, int Client_ID, SOCKET_INFO* Sockets[])
{
	int header = Set_Header(flags);

	memcpy(buffer, &header, sizeof(header));
	memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
	if (flags == 1)		// connection Ŭ���̾�Ʈ���� �ڽ��� id��ȣ�� ����
	{
	}
	else if (flags == 2)	// CLIENT_ID Ŭ���̾�Ʈ�� ����� ���� �ٸ� Ŭ���̾�Ʈ���� ���Ḧ �˸�
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

��Ŷ ����
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

���� �帧:
C1 -> S [Connection(c1)]: ���� ��û

S -> C1 [Connection(c1)]: ���� ���
S -> C_Other [Connection(c1)]: C1 ���� �˸�
S -> C1 [Connection(c_others)]: C1���� �ٸ� Ŭ������ ����
C_Other: C1 �߰�
C1: C_Other�鿡 ���� ��Ŷ �ް� �߰�


���� �帧:
C1 -> S [Disconnection(c1)]: ����

S: C1�� Ŭ���̾�Ʈ ����Ʈ���� ����
S -> C_All [Disconnection(c1)]: �ٸ� Ŭ�󿡰� C1�� ���� ������ �˸�
C_All: C1�� ���ӿ��� ����
*/