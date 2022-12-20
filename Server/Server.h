//
// Created by salah on 12/20/2022.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H


#include <winsock.h>
#include <iostream>
typedef unsigned long long ull;
using namespace  std;

class Server {
public:
    void init_server(int port) const;

private:
    int N = 10;
};


#endif //SERVER_SERVER_H
