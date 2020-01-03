#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_BUFFER        1024
#define SERVER_IP        "127.0.0.1"
#define SERVER_PORT       8282

int main(void)
{
    // Winsock Start - winsock.dll ·Îµå
    WSADATA WSAData;

    SOCKADDR_IN serverAddr;

    SOCKET listenSocket;

    if (WSAStartup(MAKEWORD(2, 0), &WSAData) != 0)
    {
        fprintf(stderr, "Load Win32.dll Fail\n");
        return 1;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "Create Socket Fail\n");
        return 1;
    }


    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);

    if (connect(listenSocket, (struct sockaddr*) & serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "Connect Faile\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    else
    {
        printf("Server Connected\n* Enter Message\n->");
    }

    while (1)
    {
        char messageBuffer[MAX_BUFFER];
        int i, bufferLen;
        for (i = 0; ; i++)
        {
            messageBuffer[i] = getchar();
            if (messageBuffer[i] == '\n')
            {
                messageBuffer[i++] = '\0';
                break;
            }
        }
        bufferLen = i;

        int sendBytes = send(listenSocket, messageBuffer, bufferLen, 0);
        if (sendBytes > 0)
        {
            printf("TRACE - Send message : %s (%d bytes)\n", messageBuffer, sendBytes);
            
            int receiveBytes = recv(listenSocket, messageBuffer, MAX_BUFFER, 0);
            if (receiveBytes > 0)
            {
                printf("TRACE - Receive message : %s (%d bytes)\n* Enter Message\n->", messageBuffer, receiveBytes);
            }
        }

    }

    closesocket(listenSocket);
    WSACleanup();

    return 0;
}