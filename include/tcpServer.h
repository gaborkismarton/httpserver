#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <regex>

#include "include/request.h"
#include "include/response.h"


namespace httpServer {
    class TcpServer {
    private:
        static const int BUFF_SIZE;
        int sock, client_sock;
        std::string ip_addr;
        int port;
        struct sockaddr_in sock_struct;
        unsigned int sock_struct_len;
        std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>> routes;

        int start_server();
        void accept_connection(int &client_sock);
        void parse_request(std::string req_text);
        void send_response(const Response &resp);
        void create_endpoint(const std::string& method, const std::string& path, std::function<void(Request&, Response&)> callback);

    public:
        TcpServer(std::string ip_addr, int port);
        ~TcpServer();
        void start_listen();
        void get(const std::string& path, std::function<void(Request&, Response&)> callback);
        void post(const std::string& path, std::function<void(Request&, Response&)> callback);
    };
}

#endif // TCPSERVER_H
