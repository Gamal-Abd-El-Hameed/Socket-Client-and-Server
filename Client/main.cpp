#include <stdio.h>
#include <winsock.h>
#include <bits/stdc++.h>
using namespace std;
#define PORT 19999


int main() {
    //Initiate the Socket environment
    WSADATA w;
    int nRet;
    sockaddr_in srv{};
    nRet = WSAStartup(MAKEWORD(2, 2), &w);
    if (nRet < 0) {
        return cout << "Cannot Initialize socket lib", -1;
    }
    //Open a socket - listener
    int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket < 0) {
        // errno is a system global variable which gets updated
        // with the last API call return value/result.
        return printf("\nCannot Initialize listener socket:%d", errno), -1;
    }

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = inet_addr("192.168.1.11");
    srv.sin_port = htons(PORT);
    memset(&(srv.sin_zero), 0, 8);

    nRet = connect(nSocket, (struct sockaddr*)&srv, sizeof(srv));
    if (nRet < 0) {
        return printf("\nCannot connect to server:%d", errno), -1;
    }

    //Keep sending the messages 
    char sBuff[1024] = { 0, };
    while (1) {
        Sleep(2000);
        cout << "\nWhat message you want to send..?\n";
        fgets(sBuff, 1023, stdin);
        send(nSocket, sBuff, strlen(sBuff), 0);
    }
}
