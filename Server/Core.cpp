#include "Core.h"
#include "DataUtil.h"
#include "ThreadUtil.h"
#include <stdio.h>
#include <queue>

using namespace std;

CCore::CCore() :
	index(0),
	addrlen(0),
	hListen(INVALID_SOCKET)
{
}

CCore::~CCore()
{
	if(world != NULL)
		delete world;
}


bool CCore::Init()
{
	// 게임 정보 초기화
	world = new CWorld();
	if (world == NULL)
	{
		puts("Create World Failed");
		return false;
	}

	// 서버 소켓 초기화
	WSADATA wsaData = {};
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);		// 시작
	if (iResult != 0)
	{
		fprintf(stderr, "WSAStartup Failed with Error : %d\n", iResult);
		return false;
	}
	printf("WSAStartup() OK!\n");



	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// IPV4 type tcp protocol
	if (hListen == INVALID_SOCKET)
	{
		fprintf(stderr, "Socket Failed With Error : %d\n", WSAGetLastError());
		WSACleanup();
		return false;
	}
	printf("Socket() OK!\n");


	SOCKADDR_IN tListenAddr = {};
	SetSOCKADDR(&tListenAddr, AF_INET, PORT, INADDR_ANY);


	iResult = bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));


	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "Bind Failed\n");
		closesocket(hListen);
		WSACleanup();
		return false;
	}
	printf("Socket Bind OK!\n");

	iResult = listen(hListen, SOMAXCONN);


	if (iResult == SOCKET_ERROR)
	{
		fprintf(stderr, "Listen Error\n");
		closesocket(hListen);
		WSACleanup();
		return false;
	}
	printf("Socket Listen OK!\n");



	SOCKET_INFO* SocketInfo;
	SocketInfo = (SOCKET_INFO*)calloc(1, sizeof(SOCKET_INFO));
	SocketInfo->socket = hListen;
	sockets[index] = SocketInfo;


	events[index] = WSACreateEvent();
	if (events[index] == WSA_INVALID_EVENT)
	{
		fprintf(stderr, "Event Create Fail\n");
		closesocket(hListen);
		WSACleanup();
		return false;
	}
	printf("WSACreateEvent() OK!\n");



	if (WSAEventSelect(hListen, events[index], FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	{
		fprintf(stderr, "Event Select Fail\n");
		closesocket(hListen);
		WSACleanup();
		return false;
	}
	printf("WSAEventSelect OK!\n");

	index++;	// Index 0 is ServerSocket
	addrlen = sizeof(SOCKADDR_IN);
	return true;
}

int CCore::Listen()
{
	WSANETWORKEVENTS NetworkEvents;

	SOCKET hClient = INVALID_SOCKET;
	SOCKADDR_IN tClientAddr = {};

	char msgBuffer[PACKET_SIZE * 10] = {};	// 클라이언트->서버 버퍼
	char buffer[PACKET_SIZE] = {};		// 서버->클라이언트 버퍼

	while (1)
	{
		eventIndex = WSAWaitForMultipleEvents(index, events, FALSE, WSA_INFINITE, FALSE);
		if (eventIndex == WSA_WAIT_FAILED)
		{
			fprintf(stderr, "Event Wait Fail\n");
			break;
		}
		else
		{
			//printf("WSAWaitForMultipleEvents() OK!\n");
		}

		// Get Information of Event Object
		if (WSAEnumNetworkEvents(sockets[eventIndex - WSA_WAIT_EVENT_0]->socket, events[eventIndex - WSA_WAIT_EVENT_0], &NetworkEvents) == SOCKET_ERROR)
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

			if (index > WSA_MAXIMUM_WAIT_EVENTS)
			{
				fprintf(stderr, "Connection Full\n");
				break;
			}

			hClient = accept(sockets[eventIndex - WSA_WAIT_EVENT_0]->socket, (SOCKADDR*)&tClientAddr, &addrlen);
			if (hClient == INVALID_SOCKET)
			{
				fprintf(stderr, "Client Accept Fail\n");
				continue;
			}

			sockets[index] = (SOCKET_INFO*)calloc(1, sizeof(SOCKET_INFO));
			sockets[index]->socket = hClient;
			sockets[index]->P_DATA.client_id = index;
			sockets[index]->last_send = clock();
			events[index] = WSACreateEvent();
			if (events[index] == WSA_INVALID_EVENT)
			{
				fprintf(stderr, "Event Create Fail\n");
				closesocket(hClient);
				closesocket(hListen);
				WSACleanup();
				return 1;
			}

			if (WSAEventSelect(hClient, events[index], FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
			{
				fprintf(stderr, "Event Select Fail\n");
				closesocket(hClient);
				closesocket(hListen);
				WSACleanup();
				return 1;
			}
			printf("%d번 클라 입장\n", index);
			// 새로 ACCEPT한 Client에게 ID 전달
			CDataUtil::SetData(buffer, sizeof(buffer), 1, index, sockets);
			for (int i = 1; i <= index; i++)		// 0번은 server이므로 1부터 시작
			{
				send(sockets[i]->socket, buffer, sizeof(buffer), 0);
				// player가 참가했음을 전달
			}
			for (int i = 1; i < index; i++)
			{
				memset(buffer, 0, sizeof(buffer));
				CDataUtil::SetData(buffer, sizeof(buffer), 1, i, sockets);
				send(sockets[index]->socket, buffer, sizeof(buffer), 0);
			}
			index++;
		}


		if (NetworkEvents.lNetworkEvents & FD_READ)
		{
			memset(buffer, 0, sizeof(buffer));
			memset(msgBuffer, 0, sizeof(msgBuffer));
			const wchar_t* ad;
			int iReceiveBytes = recv(sockets[eventIndex - WSA_WAIT_EVENT_0]->socket, msgBuffer, sizeof(msgBuffer), 0);
			int i = 0;
			while (iReceiveBytes > 0)
			{
				char SizeBuffer[1024] = {};
				memcpy(SizeBuffer, msgBuffer + i * 1024, 1024);
				iReceiveBytes -= 1024;
				i++;

				int iHeader = CDataUtil::GetHeader(SizeBuffer);

				if ((iHeader & 0xFFFF0000) != 0xD93D0000)
				{
					printf("우리 패킷이 아님 %x\n", iHeader);
				}
				else
				{
					int clientID = eventIndex - WSA_WAIT_EVENT_0;
					//printf("%d\n", Client_ID);
					sockets[clientID]->last_send = clock();
					//printf("플래그는 : %u\n", header & 0xFF);
					switch (iHeader & 0xFFFF)
					{
					case 3: {	// UpdatePlayerPos
						CDataUtil::GetPlayerPos(SizeBuffer + 8, sockets, clientID);
						CDataUtil::SetData(buffer, sizeof(buffer), 3, clientID, sockets);
						Send_All(buffer, sizeof(buffer), clientID);
						break;
					}
					case 4: {	// build something
						unsigned int structureID;
						VECTOR_INT pos;
						_SIZE size;
						CDataUtil::Get4Bytes(SizeBuffer + 8, &structureID, sizeof(structureID));
						CDataUtil::Get4Bytes(SizeBuffer + 12, &pos.x, sizeof(pos.x));
						CDataUtil::Get4Bytes(SizeBuffer + 16, &pos.z, sizeof(pos.z));
						CDataUtil::Get4Bytes(SizeBuffer + 20, &size.x, sizeof(size.x));
						CDataUtil::Get4Bytes(SizeBuffer + 24, &size.z, sizeof(size.z));

						printf("건물 건설 %u %d %d %d %d\n", structureID, pos.x, pos.z, size.x, size.z);

						if (world->AddStructure({ structureID, pos, size }))
						{
							memset(buffer, 0, sizeof(buffer));
							int header = CDataUtil::SetHeader(4);

							memcpy(buffer, &header, sizeof(header));
							memcpy(buffer + sizeof(header), &clientID, sizeof(clientID));
							memcpy(buffer + 8, &structureID, sizeof(structureID));
							memcpy(buffer + 12, &pos.x, sizeof(pos.x));
							memcpy(buffer + 16, &pos.z, sizeof(pos.z));
							send(sockets[clientID]->socket, buffer, sizeof(buffer), 0);
							Send_All(buffer, sizeof(buffer), clientID);
						}
						else
						{
							fprintf(stderr, "Flag4 AddStructure() 실패\n");
						}

						break;
					}
					case 5: {	// itemdrop
						unsigned int itemID;
						VECTOR pos;
						CDataUtil::Get4Bytes(SizeBuffer + 8, &itemID, sizeof(itemID));
						CDataUtil::Get4Bytes(SizeBuffer + 12, &pos.x, sizeof(pos.x));
						CDataUtil::Get4Bytes(SizeBuffer + 16, &pos.z, sizeof(pos.z));
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
							fprintf(stderr, "Flag5 AddItemDrop() 실패\n");
						}
						*/
						break;
					}
					case 6: {

						break;
					}
					case 7: {
						int chunkX;
						int chunkZ;
						CDataUtil::Get4Bytes(SizeBuffer + 8, &chunkX, sizeof(chunkX));
						CDataUtil::Get4Bytes(SizeBuffer + 12, &chunkZ, sizeof(chunkZ));

						printf("Receive Chunk Data[%d, %d]\n", chunkX, chunkZ);
						for (int i = 0; i < CHUNK_SIZE; i++)
						{
							memset(buffer, 0, sizeof(buffer));
							int header = CDataUtil::SetHeader(7);

							memcpy(buffer, &header, sizeof(header));
							memcpy(buffer + sizeof(header), &clientID, sizeof(clientID));
							memcpy(buffer + 8, &chunkX, sizeof(chunkX));
							memcpy(buffer + 12, &chunkZ, sizeof(chunkZ));
							memcpy(buffer + 16, &i, sizeof(i));
							memcpy(buffer + 20, &world->GetChunk(chunkX, chunkZ)->terrainData[i], sizeof(unsigned int) * CHUNK_SIZE);

							send(sockets[clientID]->socket, buffer, sizeof(buffer), 0);
						}

						break;
					}
					case 0xFF:		// String Message
						break;
					}
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
			int t = eventIndex - WSA_WAIT_EVENT_0;
			CCore::CloseSocket(t);
		}

	}
	Close();
}

void CCore::Send_All(char* buffer, size_t buffersize, int Client_ID)
{
	int i;
	for (i = 1; i < index; i++)
	{
		if (i != Client_ID)
		{
			send(sockets[i]->socket, buffer, buffersize, 0);
		}
	}
}

bool CCore::Close()
{
	closesocket(hListen);
	WSACleanup();		// 소켓 종료
	return true;
}

void CCore::CloseSocket(int CloseSocketIndex)
{

	char Buffer[PACKET_SIZE] = {};
	closesocket(sockets[CloseSocketIndex]->socket);
	free((void*)sockets[CloseSocketIndex]);

	if (WSACloseEvent(events[eventIndex]) == TRUE)
	{
		printf("Event Close Success\n");
	}
	else
	{
		fprintf(stderr, "Event Close Fail\n");
	}

	for (int i = eventIndex; i < index; i++)
	{
		sockets[i] = sockets[i + 1];
		events[i] = events[i + 1];
	}
	index--;
	printf("%d번 클라 퇴장\n", CloseSocketIndex);
	CDataUtil::SetData(Buffer, sizeof(Buffer), 2, CloseSocketIndex, sockets);
	for (int i = 0; i < index; i++)
	{
		send(sockets[i]->socket, Buffer, sizeof(Buffer), 0);	// 플레이어가 사라졌음을 모두에게 전달
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
	for (i = 1; i < index; i++)
	{
		//printf("%f초\n", (float)(T - sockets[i]->last_send) / CLOCKS_PER_SEC);
		if ((float)(T - sockets[i]->last_send) / CLOCKS_PER_SEC > 10)
		{
			printf("%d 번 클라 타임아웃\n", i);
			CCore::CloseSocket(i);
		}
	}
}
