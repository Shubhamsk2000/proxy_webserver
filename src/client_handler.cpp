#include "client_handler.h"
#include "proxy.h"
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<sys/socket.h>
#include "http_parser.h"
#include "lru_cache.h"

LRUCache cache(10);

void ClientHandler::handle(int client_fd){
    char buffer[4096];
    std::string request; 

    while(true){
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_fd, buffer, sizeof(buffer), 0);

        if(bytes <= 0){
            break;
        }

        request.append(buffer, bytes);

        if(request.find("\r\n\r\n") != std::string::npos){
            break;
        }
    }

    std::cout<<"------Request comming to Proxy: ------\n"<<request<<std::endl;

    HttpRequest req;
    if(!HttpParser::parse(request, req)){
        close(client_fd);
        return;
    }

    std::string cacheKey = req.method + ":" + req.host + ":" + req.path;

    std::string cachedResponse;

    if(cache.getNode(cacheKey, cachedResponse)){
        std::cout << "Cache HIT: \n";
        send(client_fd, cachedResponse.c_str(), cachedResponse.size(), 0);
        close(client_fd);
        return;
    }


    std::cout<< "Forwarding request to: "<<req.host<< " from proxy" << std::endl;
    std::string serverResponse;

    if(!Proxy::forwardRequest(client_fd, request, req.host, serverResponse)){
        const char* error =
            "HTTP/1.1 502 Bad Gateway\r\n"
            "Content-Length: 11\r\n"
            "\r\n"
            "Bad Gateway"; 
        
        send(client_fd, error, strlen(error), 0);
        close(client_fd);
        return;
    }

    cache.putNode(cachedResponse, serverResponse);
    std::cout << "Cache key stored: " << cacheKey <<std::endl;
    send(client_fd, serverResponse.c_str(), serverResponse.size(), 0);
}