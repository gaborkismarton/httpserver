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

    void TcpServer::send_response(const Response &resp) {
        std::ostringstream ss;
        ss << "HTTP1/1.1 ";
        ss << resp.status << resp.headers << "\r\n\r\n" << resp.body << "\r\n";
        std::string mess = ss.str();
        long sent = write(client_sock, mess.c_str(), mess.size());
        if (sent == mess.size())
            std::clog << "Server response sent to client" << std::endl;
        else
            std::cerr << "Error sending response to client" << std::endl;
        close(client_sock);
    }

    void TcpServer::create_endpoint(const std::string& method, const std::string& path, std::function<void(Request&, Response&)> callback){
        routes[method][path] = std::pair<std::string, std::function<void(Request&, Response&)>>(path, callback);
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
            if (recv < 0)
                std::cerr << "Failed to read bytes from client socket connection" << std::endl;
            std::clog << "Received request from client" << std::endl;
            parse_request(std::string(buffer));
        }
    }

    void TcpServer::parse_request(std::string req_text) {
        Request req;
        std::istringstream ss(req_text);
        std::string line;

        // Parse first line (ex.:, "GET / HTTP/1.1")
        std::getline(ss, line);
        std::istringstream firstLine(line);
        firstLine >> req.method >> req.path >> req.version;

        // Parse headers (until empty line)
        while (std::getline(ss, line) && line != "\r\n") {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string val = line.substr(colon + 1);
                // Trim whitespace from value
                val.erase(0, val.find_first_not_of("\r\n"));
                val.erase(val.find_last_not_of("\r\n") + 1);
                req.headers[key] = val;
            }
        }

        // Parse body (if exists)
        if (req.headers.find("Content-Length") != req.headers.end()) {
            int content_len = std::stoi(req.headers["Content-Length"]);
            req.body = req_text.substr(req_text.find("\r\n\r\n") + 4, content_len);
        }

        Response resp;
        
        // Send response if route exists
        if (routes.find(req.method) != routes.end()) {
            if (routes[req.method].find(req.path) != routes[req.method].end()) {
                routes[req.method][req.path].second(req, resp);
                send_response(resp);
            }
        }
    }

    void TcpServer::get(const std::string& path, std::function<void(Request&, Response&)> callback) {
        create_endpoint("GET", path, callback);
    }

    void TcpServer::post(const std::string& path, std::function<void(Request&, Response&)> callback) {
        create_endpoint("POST", path, callback);
    }
}