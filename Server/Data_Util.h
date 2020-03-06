#pragma once
#include "ServerSocket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int Get_Header(char* MessageBuffer);
void Get_Data(int buffer[], char* MessageBuffer, size_t size);

void Get_PlayerData(char* MessageBuffer, SOCKET_INFO* Sockets[]);
void Get_PlayerPos(char* MessageBuffer, SOCKET_INFO* Sockets[], int Client_ID);
void Get_Connection(char* MessageBuffer, SOCKET_INFO* Sockets[]);

void Get_4Bytes(char* Buffer, void* Data, size_t Data_size);


void Set_Data(char* buffer, size_t buffersize, int flags, int Client_ID, SOCKET_INFO* Sockets[]);
int Set_Header(int flags);

