#include <iostream>
#include "Server.h"
#define PORT 19999
int main() {

    cout << "Server starts\n";
    Server Server;
    Server.init_server(9909);
    return 0;
}