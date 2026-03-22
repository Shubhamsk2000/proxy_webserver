#include "server.h"
#include "client_handler.h"

#include<iostream>
#include<sys/socket.h>
#include<cstring>
#include <arpa/inet.h>
#include<thread>
#include<unistd.h>

ProxyServer::ProxyServer(int port) : port(port), server_fd(-1){}

void ProxyServer::start(){

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //defining server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    

    if(bind(server_fd, (sockaddr*) &server_addr, sizeof(server_addr)) < 0){
        perror("bind");
        close(server_fd);
        return;
    }

    if(listen(server_fd, 10) < 0){
        perror("listen");
        close(server_fd);
        return;
    }

    std::cout<<"\nServer listening on port "<< port << std::endl;

    while(true){
        int client_fd = accept(server_fd, nullptr, nullptr);

        if(client_fd < 0){
            perror("accept");
            continue;
        }

        std::thread newClientThread(ClientHandler::handle, client_fd);

        newClientThread.detach();
    }
}
