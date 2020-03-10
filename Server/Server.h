#pragma once

#include <WinSock2.h>
#include <time.h>
#include <vector>

using namespace std;

#pragma comment(lib, "ws2_32")

#include "Types.h"

#define PORT 8282 // 임의의 포트
#define PACKET_SIZE 1024 // 임의

#define CHUNK_SIZE 16 // 단일 청크의 크기
#define WORLD_SIZE 32 // n * n 월드 크기에서 n의 값