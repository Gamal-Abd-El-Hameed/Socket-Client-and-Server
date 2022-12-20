//
// Created by salah on 12/20/2022.
//

#include "Server.h"
//
// Created by salah on 12/20/2022.
//

#include "Server.h"
void Server::init_server(int port) const {
    struct  sockaddr_in srv{};
    fd_set fr, fw, fe;
    ull nMaxFd;
    WSADATA  ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
        cout<<"WSA Failed\n";
    else
        cout<<"WSA Success\n";

    // initialize the socket
    ull nSocket = socket(AF_INET/*IPv4*/, SOCK_STREAM/*for tcp connection*/, 0);
    if (nSocket < 0) {
        cout<<"Failed to create socket\n";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
        cout<<"Socket created successfully\n";
    }

    // initialize the environment for sockadde structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = INADDR_ANY;
    memset(&(srv.sin_zero), 0, 8);
    // Bind the socket to the local port
    ull nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));
    if (nRet < 0) {
        cout<<"Failed to bind the socket\n";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
        cout<<"Bind socket successfully\n";
    }
    // Listen the request from client (queues the requests)
    nRet = listen(nSocket, N); // backlog if more than N requests are incoming they should wait
    if(nRet < 0) {
        cout<<"Failed to listen\n";
        WSACleanup();
        closesocket(nSocket);
        exit(EXIT_FAILURE);
    }
    else {
        cout<<"Listen successfully\n";
    }
    nMaxFd = nSocket;
    struct timeval tv{};
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    while (true) {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);
        FD_SET(nSocket, &fr);
        FD_SET(nSocket, &fe);
        cout<<"Before call: "<<fr.fd_count<<endl;
        // Keep waiting for new request and processed it as prev requests
        nRet = select(nMaxFd+1, &fr, &fw, &fe, &tv);
        if (nRet > 0) {
            // when someone connects or communicate with a msg over
            // a dedicated connection
            cout << "Data on port ....... Processing now ....\n";
            //process the request
            if (FD_ISSET(nSocket, &fr)) {
                int t = sizeof(srv);
                ull connectionSocket = accept(nSocket, (struct sockaddr *) &srv, &t);
                if (connectionSocket < 0) {
                    cout<<"Failed to accept connection\n";
                    break;
                }
                else if (connectionSocket > 0){
                    cout<<"Accept connection\n";
                    send(connectionSocket, "Got it\n", 6, 0);
                    break;
                }
            }
            break;
        }
        else if (nRet == 0) {
            // No connection or any communication request made or you say
            // that none of the socket  descriptors are ready
            cout<<"Nothing on port: "<<port<<endl;
        }
        else {
            // fail and your app should show some useful msg
            cout<<"Failed\n";
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        cout<<"After call: "<<fr.fd_count<<"\n";
        Sleep(2000);
    }
}