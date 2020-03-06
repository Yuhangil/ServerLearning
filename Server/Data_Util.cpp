
#include "Data_Util.h"

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
	return header;
}

int Set_Header(int flags)
{
	int header = 0;
	char arr[4] = { flags & 0xFF , (flags >> 8) & 0xFF, 0x3D, 0xD9 };
	memcpy(&header, arr, sizeof(arr));
	return header;
}

void Set_Data(char* buffer, size_t buffersize, int flags, int Client_ID, SOCKET_INFO* Sockets[])
{
	memset(buffer, 0, buffersize);
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
		memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
		memcpy(buffer + 8, &((Sockets[Client_ID]->P_DATA).x), sizeof((Sockets[Client_ID]->P_DATA).x));
		memcpy(buffer + 12, &((Sockets[Client_ID]->P_DATA).y), sizeof((Sockets[Client_ID]->P_DATA).y));
	}
	else if (flags == 4)
	{
		// memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
	}
}

void Get_PlayerData(char* MessageBuffer, SOCKET_INFO* Sockets[])
{
	int client_id = 0;
	Get_4Bytes(MessageBuffer, &client_id, sizeof(client_id));
	Get_4Bytes(MessageBuffer + 4, &(Sockets[client_id]->P_DATA.playername), sizeof(Sockets[client_id]->P_DATA.playername));
}
void Get_PlayerPos(char* MessageBuffer, SOCKET_INFO* Sockets[], int Client_ID)
{
	Get_4Bytes(MessageBuffer, &((Sockets[Client_ID]->P_DATA).x), sizeof((Sockets[Client_ID]->P_DATA).x));
	Get_4Bytes(MessageBuffer + 4, &((Sockets[Client_ID]->P_DATA).y), sizeof((Sockets[Client_ID]->P_DATA).y));
}
void Get_Connection(char* MessageBuffer, SOCKET_INFO* Sockets[])
{
	int client_id = 0;
	Get_4Bytes(MessageBuffer, &client_id, sizeof(client_id));
	Get_4Bytes(MessageBuffer + 4, &(Sockets[client_id]->P_DATA.playername), sizeof(Sockets[client_id]->P_DATA.playername));
}