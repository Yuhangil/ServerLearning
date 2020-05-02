#include "Core.h"
#include "Server.h"


extern HANDLE hMutex;

int main()
{

    if (!GET_SINGLE(CCore)->Init())
    {
        GET_SINGLE(CCore)->DestroyInst();
        return 0;
    }

    int iRev = GET_SINGLE(CCore)->Listen();

    GET_SINGLE(CCore)->DestroyInst();
    return iRev;
}

/*

패킷 종류
Connection:
   int client_id
   str player_nickname

PlayerPos:
   int client_id
   float x
   float y

Structure Construct:
   int client_id
   int structure_id
   float structure_x
   float structure_y

Disconnection:
   int client_id

접속 흐름:
C1 -> S [Connection(c1)]: 접속 요청

S -> C1 [Connection(c1)]: 접속 허락
S -> C_Other [Connection(c1)]: C1 접속 알림
S -> C1 [Connection(c_others)]: C1에게 다른 클라정보 전달
C_Other: C1 추가
C1: C_Other들에 대한 패킷 받고 추가


종료 흐름:
C1 -> S [Disconnection(c1)]: 종료

S: C1을 클라이언트 리스트에서 삭제
S -> C_All [Disconnection(c1)]: 다른 클라에게 C1의 접속 해제를 알림
C_All: C1을 게임에서 삭제
*/