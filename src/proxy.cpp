#include "proxy.h"

#include<unistd.h>
#include<cstring>
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

int connectToHost(const std::string& host, int port);

bool Proxy::forwardRequest(int client_fd, const std::string& raw_request, const std::string& host){
    int server_fd = connectToHost(host, 80);
    if(server_fd < 0) return false;

    //sending request from client to real destination server
    if(send(server_fd, raw_request.c_str(), raw_request.size(), 0) < 0){
        perror("send to server");
        close(server_fd);
        return false;
    }

    char buffer[4096];

    //recieving and sending response to client again
    while(true){
        int bytes = recv(server_fd, buffer, sizeof(buffer), 0);
        if(bytes <= 0) break;

        send(client_fd, buffer, bytes, 0);
    }
    close(server_fd);
    return true;
}

int connectToHost(const std::string& host, int port){
    struct addrinfo hints{}, *res;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(host.c_str(), NULL, &hints, &res) != 0){
        perror("getaddrinfo");
        return -1;
    }

    struct sockaddr_in server_addr = *(struct sockaddr_in*) res->ai_addr;
    server_addr.sin_port = htons(port);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("socket");
        freeaddrinfo(res);
        return -1;
    }

    if(connect(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        close(server_fd);
        perror("connect");
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);
    return server_fd;
}