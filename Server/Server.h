//
// Created by salah on 12/20/2022.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
#define MAX_CLIENTS 50
#define BUFFER_SIZE 100000
#include <winsock.h>
#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <unistd.h>
typedef unsigned long long ull;
using namespace  std;

class Server {
public:
    int clients[MAX_CLIENTS];
    int clientIndex = 0;
    fd_set fr, fw, fe;
    struct  sockaddr_in srv{};
    ull nMaxFd;
    WSADATA  ws;
    ull nSocket;
    void init_server(int port);
    void sendMessage();
    static void *serveRequest(ull soc);
    static bool socketIsClosed(ull soc);
    static vector<string> splitRequest(string str, string token);
    static void getRequest(string req, int soc);
    static void postRequest(vector<string> req, int soc);
    static int getLen(string s);


private:
    int N = 10;

};


#endif //SERVER_SERVER_H
