#include "Core.h"
#include "Server.h"


extern HANDLE hMutex;

int main()
{
    CCore* serverCore = new CCore();

    if (serverCore->Init())
    {
        return serverCore->Listen();
    }

}

/*

��Ŷ ����
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

���� �帧:
C1 -> S [Connection(c1)]: ���� ��û

S -> C1 [Connection(c1)]: ���� ���
S -> C_Other [Connection(c1)]: C1 ���� �˸�
S -> C1 [Connection(c_others)]: C1���� �ٸ� Ŭ������ ����
C_Other: C1 �߰�
C1: C_Other�鿡 ���� ��Ŷ �ް� �߰�


���� �帧:
C1 -> S [Disconnection(c1)]: ����

S: C1�� Ŭ���̾�Ʈ ����Ʈ���� ����
S -> C_All [Disconnection(c1)]: �ٸ� Ŭ�󿡰� C1�� ���� ������ �˸�
C_All: C1�� ���ӿ��� ����
*/