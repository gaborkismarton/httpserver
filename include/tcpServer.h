#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <regex>

#include "include/request.h"
#include "include/response.h"
#include "include/utils.h"

namespace httpServer {
    class TcpServer {
    private:
        static const int BUFF_SIZE;
        static const int MAX_EVENTS;
        int sock;
        std::string ip_addr;
        int port;
        struct sockaddr_in sock_struct;
        unsigned int sock_struct_len;
        std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>> routes;

        int kq; // kqueue file descriptor
        // buffer to add incoming client data into
        std::map<int, std::string> client_buff;    

        int start_server();
        void accept_connection();
        void handle_data(int client_sock);
        void parse_request(int client_sock, std::string req_text);
        void send_response(int client_sock, Response &resp);
        void create_endpoint(const std::string& method, const std::string& path, std::function<void(Request&, Response&)> callback);
        void start_listen();

    public:
        TcpServer(std::string ip_addr, int port);
        ~TcpServer();
        void handler();
        void get(const std::string& path, std::function<void(Request&, Response&)> callback);
        void post(const std::string& path, std::function<void(Request&, Response&)> callback);
    };
}

#endif // TCPSERVER_H
