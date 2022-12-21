// SocketServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <winsock.h>

#define PORT 19999

int main()
{
    //Initiate the Socket environment
    WSADATA w;
    int nRet = 0;

    sockaddr_in srv;

    nRet = WSAStartup(MAKEWORD(2, 2), &w);
    if (nRet < 0)
    {
        printf("\nCannot Initialize socket lib");
        return -1;
    }
    //Open a socket - listener
    int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket < 0)
    {
        //errno is a system global variable which gets updated
        //with the last API call return value/result.
        printf("\nCannot Initialize listener socket:%d", errno);;
        return -1;
    }

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);
    memset(&(srv.sin_zero), 0, 8);

    nRet = bind(nSocket, (struct sockaddr*)&srv, sizeof(srv));
    if (nRet < 0)
    {
        printf("\nCannot bind at port:%d", errno);
        return -1;
    }

    nRet = listen(nSocket, 5);
    if (nRet < 0)
    {
        printf("\nCannot listen at port:%d", errno);
        return -1;
    }

    int nClient = 0;
    int addrlen = sizeof(srv);
    nClient = accept(nSocket, (struct sockaddr*)&srv, &addrlen);

    if (nRet < 0)
    {
        printf("\nCannot accept client at port:%d", errno);
        return -1;
    }

    char sBuff[1024] = { 0, };

    while (1)
    {
        memset(sBuff, 0, 1024);
        nRet = recv(nClient, sBuff, 1024, 0);
        if (nRet < 0)
        {
            printf("\nCannot recv message:%d", errno);
            return -1;
        }
        printf("\nRecieved message from client:\n%s",
            sBuff);
    }
}