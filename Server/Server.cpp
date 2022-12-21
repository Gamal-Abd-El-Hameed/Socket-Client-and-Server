//
// Created by salah on 12/20/2022.
//

#include "Server.h"
//
// Created by salah on 12/20/2022.
//

#include "Server.h"

void Server::init_server(int port) {
    //Initiate the Socket environment
    WSADATA w;
    int nRet = 0;

    sockaddr_in srv;

    nRet = WSAStartup(MAKEWORD(2, 2), &w);
    if (nRet < 0)
    {
        printf("\nCannot Initialize socket lib");
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    //Open a socket - listener
    int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket < 0)
    {
        //errno is a system global variable which gets updated
        //with the last API call return value/result.
        printf("\nCannot Initialize listener socket:%d", errno);;
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(port);
    memset(&(srv.sin_zero), 0, 8);

    nRet = bind(nSocket, (struct sockaddr*)&srv, sizeof(srv));
    if (nRet < 0)
    {
        printf("\nCannot bind at port:%d", errno);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    nRet = listen(nSocket, 5);
    if (nRet < 0)
    {
        printf("\nCannot listen at port:%d", errno);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    while(true) {
        int nClient = 0;
        int addrlen = sizeof(srv);
        cout<<"\nWaiting to accept connection";
        nClient = accept(nSocket, (struct sockaddr*)&srv, &addrlen);
        if (nClient < 0)
        {
            printf("\nCannot accept client at port:%d", errno);
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        else {
            cout<<"\nConnection accepted";
        }
        thread t(serveRequest, nClient);
        if (t.joinable())
            t.join();
    }
    closesocket(nSocket);
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
                    postRequest(request_t, soc);
                } else if (all_requests[i].find("GET") != std::string::npos) {
                    getRequest(request_t[0], soc);
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

void Server::getRequest(string req, int soc) {
    cout << "Handling get request -->" << req << endl;
    vector<string> fname = splitRequest(req, " ");
    string fpath = "pwd/Server/ServerFiles/" + fname[1];

    string response;

    ifstream file(fpath.c_str(), ifstream::ate | ifstream::binary);
    if (!file.is_open()) {
        cout << "error in file, 404\n";
        response = "HTTP/1.1 404 Not Found\r\n";
        write(soc, (char *) response.c_str(), response.size());
        return;
    } else {
        response = "HTTP/1.1 200 OK\r\n";

        long len = file.tellg();
        char *buffer = new char[len];
        memset(buffer, '\0', sizeof(buffer));

        response += ("Content-Length: " + to_string(len - 1) + "\r\n\r\n");
        write(soc, (char *) response.c_str(), response.size());

        file.seekg(0, ios::beg);
        file.read(buffer, len);
        file.close();
        cout << "file opened & read successfully";
        while (len > 0) {
            int length_sent = write(soc, buffer, len);
            if (length_sent <= 0) {
                break;
            }
            buffer += length_sent;
            len -= length_sent;
        }
        cout << "file sent successfully";
    }
}


void Server::postRequest(vector<string> req, int soc) {
    cout << "Handling post request"<< endl;
    string response = "HTTP/1.1 200 OK\r\n\r\n";
    write(soc, (char *) response.c_str(), response.size());

    vector<string> fname = splitRequest(req[0], " ");
    std::ofstream file("pwd/Server/ServerFiles/" + fname[1], ios::out | ios::binary);

    if (!file.is_open()) {
        cout << "error in file\n";
    }
    int len = getLen(req[1]);
    char *buffer = new char[len];
    memset(buffer, '\0', sizeof(buffer));

    while (len > 0) {
        int length_get = read(soc, buffer, len - 1);
        if (length_get >= len - 1 || length_get <= 0) {
            break;
        }
        buffer += length_get;
        len -= length_get;
    }
    file.write(buffer, strlen(buffer));
    file.close();
    cout << "file got successfully\n";
    cout << "file content: " << buffer << endl;
}

int Server::getLen(string s) {
    stringstream ss;
    ss << s;
    string temp;
    int found;
    while (!ss.eof()) {
        ss >> temp;
        if (stringstream(temp) >> found){
            return found;
        }
        temp = "";
    }
    return 0;
}
