#include "Core.h"
#include "DataUtil.h"
#include "ThreadUtil.h"
#include <stdio.h>

CCore::CCore() :
	m_iIndex(0),
	m_iaddrlen(0),
	m_hListen(INVALID_SOCKET)
{
}

CCore::~CCore()
{
}

bool CCore::Init()
{
	WSADATA wsaData = {};
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);		// 시작
	if (iResult != 0)
	{
		fprintf(stderr, "WSAStartup Failed with Error : %d\n", iResult);
		return false;
	}
	printf("WSAStartup() OK!\n");



	m_hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// IPV4 type tcp protocol
	if (m_hListen == INVALID_SOCKET)
	{
		fprintf(stderr, "Socket Failed With Error : %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}
	printf("Socket() OK!\n");


	SOCKADDR_IN tListenAddr = {};
	SetSOCKADDR(&tListenAddr, AF_INET, PORT, INADDR_ANY);


	iResult = bind(m_hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));


	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "Bind Failed\n");
		closesocket(m_hListen);
		WSACleanup();
		return false;
	}
	printf("Socket Bind OK!\n");

	iResult = listen(m_hListen, SOMAXCONN);


	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "Listen Error\n");
		closesocket(m_hListen);
		WSACleanup();
		return false;
	}
	printf("Socket Listen OK!\n");



	SOCKET_INFO* SocketInfo;
	SocketInfo = (SOCKET_INFO*)calloc(1, sizeof(SOCKET_INFO));
	SocketInfo->socket = m_hListen;
	m_sockets[m_iIndex] = SocketInfo;


	m_events[m_iIndex] = WSACreateEvent();
	if (m_events[m_iIndex] == WSA_INVALID_EVENT)
	{
		fprintf(stderr, "Event Create Fail\n");
		closesocket(m_hListen);
		WSACleanup();
		return false;
	}
	printf("WSACreateEvent() OK!\n");



	if (WSAEventSelect(m_hListen, m_events[m_iIndex], FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		fprintf(stderr, "Event Select Fail\n");
		closesocket(m_hListen);
		WSACleanup();
		return false;
	}
	printf("WSAEventSelect OK!\n");

	m_iIndex++;	// Index 0 is ServerSocket
	m_iaddrlen = sizeof(SOCKADDR_IN);
	return true;
}

int CCore::Listen()
{
	WSANETWORKEVENTS NetworkEvents;

	SOCKET hClient = INVALID_SOCKET;
	SOCKADDR_IN tClientAddr = {};

	char MessageBuffer[PACKET_SIZE] = {};	// 클라이언트->서버 버퍼
	char buffer[PACKET_SIZE] = {};		// 서버->클라이언트 버퍼

	while (1)
	{
		int iEventIndex = WSAWaitForMultipleEvents(m_iIndex, m_events, FALSE, WSA_INFINITE, FALSE);
		if (iEventIndex == WSA_WAIT_FAILED)
		{
			fprintf(stderr, "Event Wait Fail\n");
			break;
		}
		else
		{
			//printf("WSAWaitForMultipleEvents() OK!\n");
		}

		// Get Information of Event Object
		if (WSAEnumNetworkEvents(m_sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, m_events[iEventIndex - WSA_WAIT_EVENT_0], &NetworkEvents) == SOCKET_ERROR)
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

			if (m_iIndex > WSA_MAXIMUM_WAIT_EVENTS)
			{
				fprintf(stderr, "Connection Full\n");
				break;
			}

			hClient = accept(m_sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, (SOCKADDR*)&tClientAddr, &m_iaddrlen);
			if (hClient == INVALID_SOCKET)
			{
				fprintf(stderr, "Client Accept Fail\n");
				continue;
			}

			m_sockets[m_iIndex] = (SOCKET_INFO*)calloc(1, sizeof(SOCKET_INFO));
			m_sockets[m_iIndex]->socket = hClient;
			m_sockets[m_iIndex]->P_DATA.client_id = m_iIndex;
			m_sockets[m_iIndex]->last_send = clock();
			m_events[m_iIndex] = WSACreateEvent();
			if (m_events[m_iIndex] == WSA_INVALID_EVENT)
			{
				fprintf(stderr, "Event Create Fail\n");
				closesocket(hClient);
				closesocket(m_hListen);
				WSACleanup();
				return 1;
			}

			if (WSAEventSelect(hClient, m_events[m_iIndex], FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
			{
				fprintf(stderr, "Event Select Fail\n");
				closesocket(hClient);
				closesocket(m_hListen);
				WSACleanup();
				return 1;
			}
			printf("%d번 클라 입장\n", m_iIndex);
			// 새로 ACCEPT한 Client에게 ID 전달
			CDataUtil::SetData(buffer, sizeof(buffer), 1, m_iIndex, m_sockets);
			for (int i = 1; i <= m_iIndex; i++)		// 0번은 server이므로 1부터 시작
			{
				send(m_sockets[i]->socket, buffer, sizeof(buffer), 0);
				// player가 참가했음을 전달
			}
			for (int i = 1; i < m_iIndex; i++)
			{
				memset(buffer, 0, sizeof(buffer));
				CDataUtil::SetData(buffer, sizeof(buffer), 1, i, m_sockets);
				send(m_sockets[m_iIndex]->socket, buffer, sizeof(buffer), 0);
			}
			m_iIndex++;
		}


		if (NetworkEvents.lNetworkEvents & FD_READ)
		{
			memset(buffer, 0, sizeof(buffer));
			memset(MessageBuffer, 0, sizeof(MessageBuffer));
			const wchar_t* ad;
			int receiveBytes = recv(m_sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, MessageBuffer, sizeof(MessageBuffer), 0);
			int header = CDataUtil::GetHeader(MessageBuffer);

			if ((header & 0xFFFF0000) != 0xD93D0000)
			{
				printf("우리 패킷이 아님 %x\n", header);
			}
			else
			{
				int Client_ID = iEventIndex - WSA_WAIT_EVENT_0;;
				//printf("%d\n", Client_ID);
				m_sockets[Client_ID]->last_send = clock();
				//printf("플래그는 : %u\n", header & 0xFF);
				switch (header & 0xFFFF)
				{
				case 3:		// UpdatePlayerPos
					CDataUtil::GetPlayerPos(MessageBuffer + 8, m_sockets, Client_ID);
					printf("%.3f %.3f %.3f\n", m_sockets[Client_ID]->P_DATA.x, m_sockets[Client_ID]->P_DATA.y, m_sockets[Client_ID]->P_DATA.z);
					CDataUtil::SetData(buffer, sizeof(buffer), 3, Client_ID, m_sockets);
					Send_All(buffer, sizeof(buffer), Client_ID);
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
			int t = m_sockets[iEventIndex - WSA_WAIT_EVENT_0]->P_DATA.client_id;
			closesocket(m_sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket);

			free((void*)m_sockets[iEventIndex - WSA_WAIT_EVENT_0]);

			if (WSACloseEvent(m_events[iEventIndex]) == TRUE)
			{
				printf("Event Close Success\n");
			}
			else
			{
				fprintf(stderr, "Event Close Fail\n");
			}

			for (int i = iEventIndex; i < m_iIndex; i++)
			{
				m_sockets[i] = m_sockets[i + 1];
				m_events[i] = m_events[i + 1];
			}
			printf("%d번 클라 퇴장\n", t);
			CDataUtil::SetData(buffer, sizeof(buffer), 2, t, m_sockets);
			m_iIndex--;
			for (int i = 0; i < m_iIndex; i++)
			{
				send(m_sockets[i]->socket, buffer, sizeof(buffer), 0);	// 플레이어가 사라졌음을 모두에게 전달
			}

		}

	}
	Close();
}

void CCore::Send_All(char* buffer, size_t buffersize, int Client_ID)
{
	int i;
	for (i = 1; i < m_iIndex; i++)
	{
		if (i != Client_ID)
		{
			send(m_sockets[i]->socket, buffer, buffersize, 0);
		}
	}
}

bool CCore::Close()
{
	closesocket(m_hListen);
	WSACleanup();		// 소켓 종료
	return true;
}

void CCore::SetSOCKADDR(SOCKADDR_IN* sock_addr, int family, int port, int addr)
{
	sock_addr->sin_family = family;
	sock_addr->sin_port = htons(port);
	sock_addr->sin_addr.S_un.S_addr = htonl(addr);
}

void CCore::TimeOut()
{
	int i;
	clock_t T = clock();
	WaitForSingleObject(CThreadUtil::hMutex, INFINITE);
	for (i = 1; i < m_iIndex; i++)
	{
		if ((T - m_sockets[i]->last_send) / CLOCKS_PER_SEC)
		{
			// 소켓 종료 절차 밟기
		}
	}
	ReleaseMutex(CThreadUtil::hMutex);
}
