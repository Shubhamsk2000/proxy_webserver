#include "proxy.h"

#include<unistd.h>
#include<cstring>
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

int connectToHost(const std::string& host, int port);
std::string fixRequestPath(std::string& raw_request);

bool Proxy::forwardRequest(int client_fd, std::string& raw_request, const std::string& host, std::string& response){
    int server_fd = connectToHost(host, 80);
    if(server_fd < 0) return false;

    std::string fixed = fixRequestPath(raw_request);

    //testing
    std::cout<<"\n[TEST] Sending request from Proxy to Server...";
    //sending request from client to real destination server
    if(send(server_fd, fixed.c_str(), fixed.size(), 0) < 0){
        perror("send to server");
        close(server_fd);
        return false;
    }
    std::cout<<"\n[TEST] Sent request from Proxy to Server";

    char buffer[4096];

    std::cout<<"\n[TEST] Receiving request from Server to Proxy...";
    //recieving and sending response to client again
    while(true){
        int bytes = recv(server_fd, buffer, sizeof(buffer), 0);
        if(bytes <= 0) break;

        response.append(buffer, bytes);
    }
    std::cout<<"\n[TEST] Received request from Server to Proxy\n";
    close(server_fd);
    return true;
}

int connectToHost(const std::string& host, int port){
    std::cout<<"\n[TEST] Connecting to Server...\n";
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


std::string fixRequestPath(std::string& raw_request){
    std::string modified = raw_request;

    size_t pos = raw_request.find("GET http://");
    if(pos != std::string::npos){
        size_t start = modified.find('/', pos + 11);
        size_t end = modified.find(' ', start);
        
        if (start != std::string::npos && end != std::string::npos) {
            std::string path = modified.substr(start, end - start);
            modified.replace(pos + 4, end - (pos + 4), path);
        }
    }
    std::cout<<"\nModified path: "<<modified<<std::endl;
    return modified;
}