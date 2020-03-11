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
	delete world;
}


bool CCore::Init()
{
	// ���� ���� �ʱ�ȭ
	world = new CWorld();
	if (world == NULL)
	{
		puts("Create World Failed");
		return false;
	}

	// ���� ���� �ʱ�ȭ
	WSADATA wsaData = {};
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);		// ����
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

	char MessageBuffer[PACKET_SIZE] = {};	// Ŭ���̾�Ʈ->���� ����
	char buffer[PACKET_SIZE] = {};		// ����->Ŭ���̾�Ʈ ����

	while (1)
	{
		iEventIndex = WSAWaitForMultipleEvents(m_iIndex, m_events, FALSE, WSA_INFINITE, FALSE);
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

		CCore::TimeOut();


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
			printf("%d�� Ŭ�� ����\n", m_iIndex);
			// ���� ACCEPT�� Client���� ID ����
			CDataUtil::SetData(buffer, sizeof(buffer), 1, m_iIndex, m_sockets);
			for (int i = 1; i <= m_iIndex; i++)		// 0���� server�̹Ƿ� 1���� ����
			{
				send(m_sockets[i]->socket, buffer, sizeof(buffer), 0);
				// player�� ���������� ����
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
			int iReceiveBytes = recv(m_sockets[iEventIndex - WSA_WAIT_EVENT_0]->socket, MessageBuffer, sizeof(MessageBuffer), 0);
			int iHeader = CDataUtil::GetHeader(MessageBuffer);

			if ((iHeader & 0xFFFF0000) != 0xD93D0000)
			{
				printf("�츮 ��Ŷ�� �ƴ� %x\n", iHeader);
			}
			else
			{
				int iClientID = iEventIndex - WSA_WAIT_EVENT_0;
				//printf("%d\n", Client_ID);
				m_sockets[iClientID]->last_send = clock();
				//printf("�÷��״� : %u\n", header & 0xFF);
				switch (iHeader & 0xFFFF)
				{
				case 3:		// UpdatePlayerPos
					CDataUtil::GetPlayerPos(MessageBuffer + 8, m_sockets, iClientID);
					CDataUtil::SetData(buffer, sizeof(buffer), 3, iClientID, m_sockets);
					Send_All(buffer, sizeof(buffer), iClientID);
					break;
				case 4:		// build something
				{
					unsigned int iStructureID;
					VECTOR_INT tPos;
					_SIZE size;
					CDataUtil::Get4Bytes(MessageBuffer + 8, &iStructureID, sizeof(iStructureID));
					CDataUtil::Get4Bytes(MessageBuffer + 12, &tPos.x, sizeof(tPos.x));
					CDataUtil::Get4Bytes(MessageBuffer + 16, &tPos.z, sizeof(tPos.z));
					CDataUtil::Get4Bytes(MessageBuffer + 20, &size.x, sizeof(size.x));
					CDataUtil::Get4Bytes(MessageBuffer + 24, &size.z, sizeof(size.z));

					printf("�ǹ� �Ǽ� %u %d %d %d %d\n", iStructureID, tPos.x, tPos.z, size.x, size.z);

					if (world->AddStructure({ iStructureID, tPos, size }))
					{
						memset(buffer, 0, sizeof(buffer));
						int header = CDataUtil::SetHeader(4);

						memcpy(buffer, &header, sizeof(header));
						memcpy(buffer + sizeof(header), &iClientID, sizeof(iClientID));
						memcpy(buffer + 8, &iStructureID, sizeof(iStructureID));
						memcpy(buffer + 12, &tPos.x, sizeof(tPos.x));
						memcpy(buffer + 16, &tPos.z, sizeof(tPos.z));
						send(m_sockets[iClientID]->socket, buffer, sizeof(buffer), 0);
						Send_All(buffer, sizeof(buffer), iClientID);
					}
					else
					{
						fprintf(stderr, "Flag4 AddStructure() ����\n");
					}

					break;
				}
				case 5:	// itemdrop
					unsigned int itemID;
					VECTOR tPos;
					CDataUtil::Get4Bytes(MessageBuffer + 8, &itemID, sizeof(itemID));
					CDataUtil::Get4Bytes(MessageBuffer + 12, &tPos.x, sizeof(tPos.x));
					CDataUtil::Get4Bytes(MessageBuffer + 16, &tPos.z, sizeof(tPos.z));
					/* 

					if (world->AddItemDrop({itemID, tPos}))
					{
						memset(buffer, 0, sizeof(buffer));
						int header = CDataUtil::SetHeader(5);
						memcpy(buffer, &header, sizeof(header));
						memcpy(buffer + sizeof(header), &iClientID, sizeof(iClientID));
						memcpy(buffer + 8, &itemID, sizeof(itemID));
						memcpy(buffer + 12, &Pos.x, sizeof(Pos.x));
						memcpy(buffer + 16, &Pos.z, sizeof(Pos.z));
						send(m_sockets[iClientID]->socket, buffer, sizeof(buffer), 0);
						Send_All(buffer, sizeof(buffer), iClientID);
					}
					else
					{
						fprintf(stderr, "Flag5 AddItemDrop() ����\n");
					}
					*/
					break;
				case 6:
					break;
				case 7:
					break;
				case 0xFF:		// String Message
					break;
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
			int t = iEventIndex - WSA_WAIT_EVENT_0;
			CCore::CloseSocket(t);
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
	WSACleanup();		// ���� ����
	return true;
}

void CCore::CloseSocket(int CloseSocketIndex)
{

	char Buffer[PACKET_SIZE] = {};
	closesocket(m_sockets[CloseSocketIndex]->socket);
	free((void*)m_sockets[CloseSocketIndex]);

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
	m_iIndex--;
	printf("%d�� Ŭ�� ����\n", CloseSocketIndex);
	CDataUtil::SetData(Buffer, sizeof(Buffer), 2, CloseSocketIndex, m_sockets);
	for (int i = 0; i < m_iIndex; i++)
	{
		send(m_sockets[i]->socket, Buffer, sizeof(Buffer), 0);	// �÷��̾ ��������� ��ο��� ����
	}
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
	for (i = 1; i < m_iIndex; i++)
	{
		printf("%f��\n", (float)(T - m_sockets[i]->last_send) / CLOCKS_PER_SEC);
		if ((float)(T - m_sockets[i]->last_send) / CLOCKS_PER_SEC > 10)
		{
			printf("%d �� Ŭ�� Ÿ�Ӿƿ�\n", i);
			CCore::CloseSocket(i);
		}
	}
}
