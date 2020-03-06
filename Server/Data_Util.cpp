
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
	printf("%x\n", header);
	return header;
}

int Set_Header(int flags)
{
	int header = 0;
	char arr[4] = { flags & 0xFF , (flags >> 8) & 0xFF, 0x3D, 0xD9 };
	memcpy(&header, arr, sizeof(arr));
	return header;
}

void Set_Data(char* buffer, int flags, int Client_ID, SOCKET_INFO* Sockets[])
{
	int header = Set_Header(flags);

	memcpy(buffer, &header, sizeof(header));
	if (flags == 1)		// connection Ŭ���̾�Ʈ���� �ڽ��� id��ȣ�� ����
	{
		memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
	}
	else if (flags == 2)	// CLIENT_ID Ŭ���̾�Ʈ�� ����� ���� �ٸ� Ŭ���̾�Ʈ���� ���Ḧ �˸�
	{
		memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
	}
	else if (flags == 3)
	{
		memcpy(buffer + sizeof(header), &Client_ID, sizeof(Client_ID));
		memcpy(buffer + 8, &Sockets[Client_ID]->P_DATA.x, sizeof(Sockets[Client_ID]->P_DATA.x));
		memcpy(buffer + 12, &Sockets[Client_ID]->P_DATA.y, sizeof(Sockets[Client_ID]->P_DATA.y));
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
void Get_PlayerPos(char* MessageBuffer, SOCKET_INFO* Sockets[])
{
	int client_id = 0;
	Get_4Bytes(MessageBuffer, &client_id, sizeof(client_id));
	Get_4Bytes(MessageBuffer + 4, &(Sockets[client_id]->P_DATA.x), sizeof(Sockets[client_id]->P_DATA.x));
	Get_4Bytes(MessageBuffer + 8, &(Sockets[client_id]->P_DATA.y), sizeof(Sockets[client_id]->P_DATA.y));
	Get_4Bytes(MessageBuffer + 12, &(Sockets[client_id]->P_DATA.z), sizeof(Sockets[client_id]->P_DATA.z));
	Get_4Bytes(MessageBuffer + 16, &(Sockets[client_id]->P_DATA.dx), sizeof(Sockets[client_id]->P_DATA.dx));
	Get_4Bytes(MessageBuffer + 20, &(Sockets[client_id]->P_DATA.dy), sizeof(Sockets[client_id]->P_DATA.dy));
	Get_4Bytes(MessageBuffer + 24, &(Sockets[client_id]->P_DATA.dz), sizeof(Sockets[client_id]->P_DATA.dz));
}
void Get_Connection(char* MessageBuffer, SOCKET_INFO* Sockets[])
{
	int client_id = 0;
	Get_4Bytes(MessageBuffer, &client_id, sizeof(client_id));
	Get_4Bytes(MessageBuffer + 4, &(Sockets[client_id]->P_DATA.playername), sizeof(Sockets[client_id]->P_DATA.playername));
}