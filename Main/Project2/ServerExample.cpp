#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 8282 // ������ ��Ʈ
#define PACKET_SIZE 1024 // ����


int main(void)
{
	WSADATA wsaData = {};
	SOCKET hListen = {};
	SOCKET hClient = {};
	SOCKADDR_IN tListenAddr = {};
	SOCKADDR_IN tClientAddr = {};

	int iClientSize = sizeof(tClientAddr);


	WSAStartup(MAKEWORD(2, 2), &wsaData);		// ����

	
	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);		// IPV4 type tcp protocol
	tListenAddr.sin_family = AF_INET;
	tListenAddr.sin_port = htons(PORT);
	tListenAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	bind(hListen, (SOCKADDR*)& tListenAddr, sizeof(tListenAddr));
	listen(hListen, SOMAXCONN);

	hClient = accept(hListen, (SOCKADDR*)& tClientAddr, &iClientSize);
	////////////////////////////////////////////////////



	// �޽��� ���޺�



	/////////////////////////////////////////////////////
	closesocket(hClient);
	closesocket(hListen);
	WSACleanup();		// ���� ����
	return 0;
}