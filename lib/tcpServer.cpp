#include "include/tcpServer.h"

namespace httpServer {
    const int TcpServer::BUFF_SIZE = 30720;
    int TcpServer::start_server() {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Cannot create socket" << std::endl;
            return 1;
        }
        if (bind(sock, (sockaddr *)&sock_struct, sock_struct_len) < 0) {
            std::cerr << "Cannot bind socket to address" << std::endl;
            return 1;
        }
        return 0;
    }

    void TcpServer::accept_connection(int &client_sock) {
        client_sock = accept(sock, (sockaddr *)&sock_struct, &sock_struct_len);
        if (client_sock < 0)
            std::cerr << "Server failed to accept incoming connection from address: " << inet_ntoa(sock_struct.sin_addr) << "; port: " << ntohs(sock_struct.sin_port) << std::endl;
    }

    void TcpServer::start_listen() {
        if (listen(sock, 20) < 0)
            std::cerr << "Socket listen failed" << std::endl;
        std::clog << "Listening on address: " << inet_ntoa(sock_struct.sin_addr) << " port: " << ntohs(sock_struct.sin_port) << std::endl;
        int recv;
        while (true) {
            std::clog << "Waiting for a new connection..." << std::endl;
            accept_connection(client_sock);
            char buffer[TcpServer::BUFF_SIZE] = {0};
            recv = read(client_sock, buffer, TcpServer::BUFF_SIZE);
            if (recv < 0) {
                std::cerr << "Failed to read bytes from client socket connection" << std::endl;
            }
            std::clog << "Received request from client" << std::endl;
            std::cout << buffer << std::endl;
            sendResponse();
            close(client_sock);
        }
    }

    void TcpServer::sendResponse() {
        std::string html = "<!DOCTYPE html><html lang=\"en\"><body>Hello World!</body></html>";
        std::ostringstream ss;
        ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << html.size() << "\n\n" << html;
        std::string mess = ss.str();
        long sent = write(client_sock, mess.c_str(), mess.size());

        if (sent == mess.size())
            std::clog << "Server response sent to client" << std::endl;
        else
            std::cerr << "Error sending response to client" << std::endl;
    }

    TcpServer::TcpServer(std::string ip_addr, int port): ip_addr(ip_addr), port(port), sock_struct_len(sizeof(sock_struct)) {
        sock_struct.sin_family = AF_INET;
        sock_struct.sin_port = htons(port);
        sock_struct.sin_addr.s_addr = inet_addr(ip_addr.c_str());

        if (start_server() != 0)
            std::cerr << "Failed to start server with port: " << port << std::endl;
    }

    TcpServer::~TcpServer() {
        close(sock);
        exit(0);
    }
}