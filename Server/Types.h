#pragma once

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
	float x, y, z;
	float dx, dy, dz;
}PLAYER_DATA;

typedef struct STRUCTURE_DATA
{
	unsigned int structure_id;
	float x, y;
}STRUCTURE_DATA;

typedef struct SOCKET_INFO
{
	SOCKET socket;
	DATA message;
	int receiveBytes;
	int sendBytes;
	clock_t last_send;
	PLAYER_DATA P_DATA;

} SOCKET_INFO;
