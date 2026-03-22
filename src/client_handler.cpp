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

    std::cout << "\n---- HTTP REQUEST START ----\n";
    HttpRequest req;
    if(!HttpParser::parse(request, req)){
        close(client_fd);
        return;
    }

    std::string cacheKey = req.method + ":" + req.host + ":" + req.path;

    std::cout << "Cache key stored: " << cacheKey;

    std::cout << "---- HTTP REQUEST END ----\n";

    std::cout<< "Forwarding request to: "<<req.host<<std::endl;

    if(!Proxy::forwardRequest(client_fd, request, req.host)){
        const char* error =
            "HTTP/1.1 502 Bad Gateway\r\n"
            "Content-Length: 11\r\n"
            "\r\n"
            "Bad Gateway"; 
        
        send(client_fd, error, strlen(error), 0);
    }
}