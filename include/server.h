#ifndef SERVER_H
#define SERVER_H

class ProxyServer{
    private:
        int port;
        int server_fd;

    public:
        ProxyServer(int port);
        void start();
};

#endif