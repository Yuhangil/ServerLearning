#include "DataUtil.h"

void CDataUtil::Get4Bytes(char* Buffer, void* Data, size_t Data_size)
{
	memcpy(Data, Buffer, Data_size);
}

int CDataUtil::GetHeader(char* MessageBuffer)
{
	char buff[4] = {};
	int header = 0;
	int t = 0;
	Get4Bytes(MessageBuffer, buff, sizeof(buff));
	t = buff[0];
	buff[0] = buff[3];
	buff[3] = t;
	t = buff[1];
	buff[1] = buff[2];
	buff[2] = t;
	Get4Bytes(buff, &header, sizeof(header));
	return header;
}

void CDataUtil::GetPlayerData(char* MessageBuffer, SOCKET_INFO* Sockets[])
{
	int client_id = 0;
	Get4Bytes(MessageBuffer, &client_id, sizeof(client_id));
	Get4Bytes(MessageBuffer + 4, &(Sockets[client_id]->P_DATA.playername), sizeof(Sockets[client_id]->P_DATA.playername));
}

void CDataUtil::GetPlayerPos(char* MessageBuffer, SOCKET_INFO* Sockets[], int Client_ID)
{
	Get4Bytes(MessageBuffer, &((Sockets[Client_ID]->P_DATA).x), sizeof((Sockets[Client_ID]->P_DATA).x));
	Get4Bytes(MessageBuffer + 4, &((Sockets[Client_ID]->P_DATA).y), sizeof((Sockets[Client_ID]->P_DATA).y));
	Get4Bytes(MessageBuffer + 8, &((Sockets[Client_ID]->P_DATA).z), sizeof((Sockets[Client_ID]->P_DATA).z));
	Get4Bytes(MessageBuffer + 12, &((Sockets[Client_ID]->P_DATA).dx), sizeof((Sockets[Client_ID]->P_DATA).dx));
	Get4Bytes(MessageBuffer + 16, &((Sockets[Client_ID]->P_DATA).dy), sizeof((Sockets[Client_ID]->P_DATA).dy));
	Get4Bytes(MessageBuffer + 20, &((Sockets[Client_ID]->P_DATA).dz), sizeof((Sockets[Client_ID]->P_DATA).dz));
}

void CDataUtil::GetConnection(char* MessageBuffer, SOCKET_INFO* Sockets[])
{
	int client_id = 0;
	Get4Bytes(MessageBuffer, &client_id, sizeof(client_id));
	Get4Bytes(MessageBuffer + 4, &(Sockets[client_id]->P_DATA.playername), sizeof(Sockets[client_id]->P_DATA.playername));
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
	{
		memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
		memcpy(buffer + 8, &((Sockets[Client_ID]->P_DATA).x), sizeof((Sockets[Client_ID]->P_DATA).x));
		memcpy(buffer + 12, &((Sockets[Client_ID]->P_DATA).y), sizeof((Sockets[Client_ID]->P_DATA).y));
		memcpy(buffer + 16, &((Sockets[Client_ID]->P_DATA).z), sizeof((Sockets[Client_ID]->P_DATA).z));
		memcpy(buffer + 20, &((Sockets[Client_ID]->P_DATA).dx), sizeof((Sockets[Client_ID]->P_DATA).dx));
		memcpy(buffer + 24, &((Sockets[Client_ID]->P_DATA).dy), sizeof((Sockets[Client_ID]->P_DATA).dy));
		memcpy(buffer + 28, &((Sockets[Client_ID]->P_DATA).dz), sizeof((Sockets[Client_ID]->P_DATA).dz));
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
