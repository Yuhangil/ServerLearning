#pragma once

#include <WinSock2.h>
#include <time.h>
#include <vector>

using namespace std;

#pragma comment(lib, "ws2_32")

#include "Types.h"

#define PORT 8282 // ������ ��Ʈ
#define PACKET_SIZE 1024 // ����

#define CHUNK_SIZE 16 // ���� ûũ�� ũ��
#define WORLD_SIZE 32 // n * n ���� ũ�⿡�� n�� ��