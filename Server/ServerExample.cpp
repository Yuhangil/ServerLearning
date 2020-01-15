#include <stdio.h>
#include <WinSock2.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32")

#define PORT 8282 // 임의의 포트
#define PACKET_SIZE 1024 // 임의

typedef struct SOCKET_DATA
{
	int header; // 0~7 bit checksum , 8~23bit data length, 24~31 bit data type
	char Message[512];
}DATA;


typedef struct SOCKET_INFO
{
	SOCKET socket;
	DATA message;
	int receiveBytes;
	int sendBytes;
} SOCKET_INFO;

void Set_SOCKADDR(SOCKADDR_IN* sock_addr, int family, int port, int addr);
void Set_Data(DATA* Socket_data, char flag, short data_size);

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

	char MessageBuffer[PACKET_SIZE + 1];

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
			iIndex++;
		}
		if (NetworkEvents.lNetworkEvents & FD_READ)
		{
			int receiveBytes = recv(Sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, MessageBuffer, PACKET_SIZE, 0);
			if (receiveBytes > 0)
			{
				printf("TRACE - Receive Message : %s (%d bytes)\n", MessageBuffer, receiveBytes);
			}
		}
		if (NetworkEvents.lNetworkEvents & FD_WRITE)
		{
			int SendBytes = send(Sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, MessageBuffer, sizeof(MessageBuffer), 0);
			if (SendBytes > 0)
			{
				printf("TRACE - Send Message : %s (%d bytes)\n", MessageBuffer, SendBytes);
			}
		}
		if (NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
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
			iIndex--;
		}

	}

	////////////////////////////////////////////////////



	// 메시지 전달부



	/////////////////////////////////////////////////////
	closesocket(hListen);
	WSACleanup();		// 소켓 종료
	return 0;
}

void Set_SOCKADDR(SOCKADDR_IN* sock_addr, int family, int port, int addr)
{
	sock_addr->sin_family = family;
	sock_addr->sin_port = htons(port);
	sock_addr->sin_addr.S_un.S_addr = htonl(addr);
}
void Set_Data(DATA* Socket_data, char flag, short data_size, char arr[])
{
	Socket_data->header = Socket_data->header & 157;	// checksum
	Socket_data->header = Socket_data->header & (flag << 24);
	Socket_data->header = Socket_data->header & (data_size << 8);
	memcpy(Socket_data->Message, arr, sizeof(arr));
}