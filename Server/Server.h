#pragma once

#include <WinSock2.h>
#include <time.h>
#include <vector>


#pragma comment(lib, "ws2_32")

#include "Types.h"

#define PORT 8282 // 임의의 포트
#define PACKET_SIZE 1024 // 임의

#define CHUNK_SIZE 16 // 단일 청크의 크기
#define CHUNKS_PER_WORLD 32 // n * n 월드 크기에서 n의 값

#define INV_ROW 5
#define INV_COLUMN 10