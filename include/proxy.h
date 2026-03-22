#ifndef PROXY_h
#define PROXY_H

#include<string>

class Proxy{
    public:
        static bool forwardRequest(int client_fd, std::string& raw_reqest, const std::string& host, std::string& response);
};
#endif
