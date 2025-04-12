#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <sstream>


namespace httpServer {
    class TcpServer {
    private:
        static const int BUFF_SIZE;
        int sock, client_sock;
        std::string ip_addr;
        int port;
        struct sockaddr_in sock_struct;
        unsigned int sock_struct_len;
        int start_server();
        void accept_connection(int &client_sock);
        void sendResponse();

    public:
        TcpServer(std::string ip_addr, int port);
        ~TcpServer();
        void start_listen();
    };
}

#endif // TCPSERVER_H
