#pragma once

typedef struct _tagVECTOR
{
	float x, y, z;
}VECTOR;

typedef struct _tagVECTOR_INT
{
	int x, y, z;
	_tagVECTOR_INT() :
		x(0), y(0), z(0)
	{	}
	_tagVECTOR_INT(int _x, int _y, int _z) :
		x(_x), y(_y), z(_z)
	{	}
	_tagVECTOR_INT(int _x, int _z) :
		x(_x), y(0), z(_z)
	{	}
}VECTOR_INT, _SIZE;

typedef struct SOCKET_DATA
{
	int header; // 0~7 bit checksum , 8~23bit data length, 24~31 bit data type
	char Message[512];
}DATA;

typedef struct PLAYER_DATA
{
	char playername[31];
	int client_id;
	VECTOR pos;
	VECTOR velocity;
}PLAYER_DATA;

typedef struct STRUCTURE_DATA
{
	unsigned int structure_id;
	VECTOR_INT pos;
	_SIZE size;
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


