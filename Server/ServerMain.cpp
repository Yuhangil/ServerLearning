#include "Data_Util.h"
#include "ServerSocket.h"
#include "ThreadUtil.h"
#include <locale.h>
#include <Windows.h>


extern HANDLE hMutex;

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

	char MessageBuffer[PACKET_SIZE] = {};	// 클라이언트->서버 버퍼
	char buffer[PACKET_SIZE] = {};		// 서버->클라이언트 버퍼

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);		// 시작
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
			Sockets[iIndex]->last_send = clock();
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
			printf("%d번 클라 입장\n", iIndex);
			// 새로 ACCEPT한 Client에게 ID 전달
			Set_Data(buffer, sizeof(buffer), 1, iIndex, Sockets);
			for (int i = 1; i <= iIndex; i++)		// 0번은 server이므로 1부터 시작
			{
				send(Sockets[i]->socket, buffer, sizeof(buffer), 0);
				// player가 참가했음을 전달
			}
			for (int i = 1; i < iIndex; i++)
			{
				memset(buffer, 0, sizeof(buffer));
				Set_Data(buffer, sizeof(buffer), 1, i, Sockets);
				send(Sockets[iIndex]->socket, buffer, sizeof(buffer), 0);
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
				printf("우리 패킷이 아님 %x\n", header);
			}
			else
			{
				int Client_ID = iEventIndex - WSA_WAIT_EVENT_0;;
				printf("%d\n", Client_ID);
				Sockets[Client_ID]->last_send = clock();
				printf("플래그는 : %u\n", header & 0xFF);
				switch (header & 0xFFFF)
				{
				case 3:		// UpdatePlayerPos
					Get_PlayerPos(MessageBuffer + 8, Sockets, Client_ID);
					Set_Data(buffer, sizeof(buffer), 3, Client_ID, Sockets);
					Send_All(buffer, sizeof(buffer), Sockets, iIndex, Client_ID);
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
			printf("%d번 클라 퇴장\n", t);
			Set_Data(buffer, sizeof(buffer), 2, t, Sockets);
			iIndex--;
			for (int i = 0; i < iIndex; i++)
			{
				 send(Sockets[i]->socket, buffer, sizeof(buffer), 0);	// 플레이어가 사라졌음을 모두에게 전달
			}
			
		}

	}

	closesocket(hListen);
	WSACleanup();		// 소켓 종료
	return 0;
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