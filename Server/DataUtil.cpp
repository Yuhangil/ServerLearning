#include "DataUtil.h"

void CDataUtil::GetBytes(char* Buffer, void* Data, size_t Data_size)
{
	memcpy(Data, Buffer, Data_size);
}

int CDataUtil::GetHeader(char* MessageBuffer)
{
	char buff[4] = {};
	int header = 0;
	int t = 0;
	GetBytes(MessageBuffer, buff, sizeof(buff));
	t = buff[0];
	buff[0] = buff[3];
	buff[3] = t;
	t = buff[1];
	buff[1] = buff[2];
	buff[2] = t;
	GetBytes(buff, &header, sizeof(header));
	return header;
}

void CDataUtil::SetData(char* buffer, size_t buffersize, int flags, int Client_ID, SOCKET_INFO* Sockets[])
{
	memset(buffer, 0, buffersize);
	int header = SetHeader(flags);

	memcpy(buffer, &header, sizeof(header));
	memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
	if (flags == 1)		// connection 클라이언트에게 자신의 id번호를 전달
	{
	}
	else if (flags == 2)	// CLIENT_ID 클라이언트의 종료로 인해 다른 클라이언트에게 종료를 알림
	{
	}
	else if (flags == 3)
	{/*
		memcpy(buffer + 8, &((Sockets[Client_ID]->P_DATA).pos.x), sizeof((Sockets[Client_ID]->P_DATA).pos.x));
		memcpy(buffer + 12, &((Sockets[Client_ID]->P_DATA).pos.y), sizeof((Sockets[Client_ID]->P_DATA).pos.y));
		memcpy(buffer + 16, &((Sockets[Client_ID]->P_DATA).pos.z), sizeof((Sockets[Client_ID]->P_DATA).pos.z));
		memcpy(buffer + 20, &((Sockets[Client_ID]->P_DATA).velocity.x), sizeof((Sockets[Client_ID]->P_DATA).velocity.x));
		memcpy(buffer + 24, &((Sockets[Client_ID]->P_DATA).velocity.y), sizeof((Sockets[Client_ID]->P_DATA).velocity.y));
		memcpy(buffer + 28, &((Sockets[Client_ID]->P_DATA).velocity.z), sizeof((Sockets[Client_ID]->P_DATA).velocity.z));*/
	}
	else if (flags == 4)
	{
		// memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
	}
}

int CDataUtil::SetHeader(int iflags)
{
	int header = 0;
	char arr[4] = { iflags & 0xFF , (iflags >> 8) & 0xFF, 0x3D, 0xD9 };
	memcpy(&header, arr, sizeof(arr));
	return header;
}

int CDataUtil::ContactHeader(char* buffer, int clientID, int iflags)
{
	memset(buffer, 0, sizeof(buffer));
	int header = SetHeader(iflags);
	memcpy(buffer, &header, sizeof(header));
	memcpy(buffer + sizeof(header), &clientID, sizeof(clientID));
	return 0;
}

void CDataUtil::DumpPacket(char* buffer, int size)
{
	int* arr = new int[size];
	for (int i = 0; i < size; i++)
	{
		CDataUtil::GetBytes(buffer + i * 4, arr + i, sizeof(int));
		printf("Arr[%d]: %d, ", i, arr[i]);
	}
	printf("\n");

	delete[] arr;
}
