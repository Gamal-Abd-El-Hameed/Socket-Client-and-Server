//
// Created by salah on 12/20/2022.
//

#include "Server.h"
//
// Created by salah on 12/20/2022.
//

#include "Server.h"

void Server::init_server(int port) {
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
        cout<<"WSA Failed\n";
    else
        cout<<"WSA Success\n";
    // initialize the socket
    nSocket = socket(AF_INET/*IPv4*/, SOCK_STREAM/*for tcp connection*/, 0);
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
        // for more connections
        for (int i = clientIndex; i < MAX_CLIENTS; i++) {
            if (clients[i] != 0) {
                FD_SET(clients[i], &fr);
                FD_SET(clients[i], &fe);
            }
        }
        cout<<"Before call: "<<fr.fd_count<<endl;
        // Keep waiting for new request and processed it as prev requests
        nRet = select(nMaxFd+1, &fr, &fw, &fe, &tv);
        if (nRet > 0) {
            // when someone connects or communicate with a msg over
            // a dedicated connection
            cout << "Data on port ....... Processing now ....\n";
            //process the request
            Server::sendMessage();

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
        cout<<"After call: "<<fr.fd_count<<"\n\n\n";
        Sleep(2000);
    }
}

void Server::sendMessage() {
    if (FD_ISSET(nSocket, &fr)) {
        int t = sizeof(struct sockaddr);
        ull connectionSocket = accept(nSocket, (struct sockaddr *) &srv, &t);
        if (connectionSocket < 0) {
            cout<<"Failed to accept connection\n";
            exit(EXIT_FAILURE);
        }
        else if (connectionSocket > 0){
            cout<<"Accept connection\n";
            thread th(serveRequest, connectionSocket);
//            if (clientIndex == MAX_CLIENTS) {
//                cout<<"No more clients\n";
//            }
//            clients[clientIndex++] = connectionSocket;
//            send(connectionSocket, "Got it\n", 6, 0);
//            exit(EXIT_FAILURE);
        }

    }
    else {
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (FD_ISSET(clients[i], &fr)) {
                // got the new msg from the client
                // just rcv new msg
                // just queue that for new works of your server to fulfill the request
            }
        }
    }
}

void *Server::serveRequest(ull soc) {
    cout<<"Connection between client and server has started\nServing request\n";
    while(true) {
        string request;
        if (socketIsClosed(soc))
            break;
        vector<char> server_response(BUFFER_SIZE, 0);
        int read_size = recv(soc, &server_response[0], BUFFER_SIZE, 0);
        if (read_size <= 0)
            break;
        request += string(server_response.begin(), server_response.begin() + std::min(read_size, BUFFER_SIZE));
        if (request.find("\r\n\r\n") != std::string::npos) {
            vector<string> all_requests = splitRequest(request, "\r\n\r\n");
            for (int i = 0; i < all_requests.size(); i++) {
                cout << request + "\n";
                if (all_requests[i].empty()) {
                    continue;
                }
                cout << "serving request \n";
                vector<string> request_t = splitRequest(all_requests[i], "\r\n");
                if (all_requests[i].find("POST") != std::string::npos) {
//                    post_request(request_t, soc);
                } else if (all_requests[i].find("GET") != std::string::npos) {
//                    get_request(request_t[0], soc);
                }
            }
        }
    }
    return nullptr;
}
bool Server::socketIsClosed(ull soc) {
    char temp;
    interrupted:
    ssize_t r = ::recv(soc, &temp, 1, MSG_PEEK);
    if (r < 0) {
        switch (errno) {
            case EINTR:
                goto interrupted;
            case EAGAIN:
                /*empty rx queue*/
                break;
            case ENOTCONN:
                /* not connected yet */
                break;
            default:
                throw (errno);
        }
    }
    return r == 0;
}

vector<string> Server::splitRequest(string str, string token) {
    vector<string> result;
    while (str.size()) {
        int index = str.find(token);
        if (index != string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0)result.push_back(str);
        } else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}



/////////////////////////////////////////////////////
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