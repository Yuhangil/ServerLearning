#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 8282 // ������ ��Ʈ
#define PACKET_SIZE 1024 // ����
#define SERVER_IP	"127.0.0.1"		// loop back


int main(void)
{
	WSADATA wsaData = {};
	SOCKET hClient = {};
	SOCKADDR_IN tAddr = {};

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	hClient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);


	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);

	connect(hClient, (SOCKADDR*)&tAddr, sizeof(tAddr));
	
	////////////////////////////////////////////////////



	// �޽��� ���޺�



	/////////////////////////////////////////////////////
	closesocket(hClient);
	WSACleanup();		// ���� ����
	return 0;
}