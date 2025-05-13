#include "include/tcpServer.h"

namespace httpServer {
    // Buffer size
    const int TcpServer::BUFF_SIZE = 30720;
    // Maximum event count in the kernel queue
    const int TcpServer::MAX_EVENTS = 64;
    
    int TcpServer::start_server() {
        // Socket creation and initialization
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "Cannot create socket" << std::endl;
            return 1;
        }
        // Binding the socket to the given port
        if (bind(sock, (sockaddr *)&sock_struct, sock_struct_len) < 0) {
            std::cerr << "Cannot bind socket to address" << std::endl;
            return 1;
        }
        return 0;
    }

    void TcpServer::accept_connection() {
        // Accepting an incoming connection
        int client_sock = accept(sock, (sockaddr *)&sock_struct, &sock_struct_len);
        if (client_sock < 0)
            std::cerr << "Server failed to accept incoming connection from address: " << inet_ntoa(sock_struct.sin_addr) << "; port: " << ntohs(sock_struct.sin_port) << std::endl;
        
        // Get the current flags for the client socket
        int flags = fcntl(client_sock, F_GETFL, 0);
        // Set the client socket to non-blocking mode
        fcntl(client_sock, F_SETFL, flags | O_NONBLOCK);

        // Create a kernel event for the socket change
        struct kevent event;

        // Configure the event for the client socket
        // EVFILT_READ checks for available data to read (filter for the event)
        // EV_ADD adds the event to the kqueue (action flag for the kqueue)
        // other parameters unused
        EV_SET(&event, client_sock, EVFILT_READ, EV_ADD, 0, 0, NULL);

        // Register the event with the kqueue
        // Only register one event, not waiting for event triggers, no timeout
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
            std::cerr << "Failed to add client socket to kqueue" << std::endl;
            close(client_sock);
        }

        // Create an empty buffer for the client
        client_buff[client_sock] = "";
        std::clog << "New connection from: " << inet_ntoa(sock_struct.sin_addr) << ":" << ntohs(sock_struct.sin_port) << std::endl;
    }

    void TcpServer::handle_data(int client_sock) {
        // Create a temporary buffer for the incoming data
        char *buffer = new char[TcpServer::BUFF_SIZE];
        // Fill up the buffer with the data
        int recv = read(client_sock, buffer, TcpServer::BUFF_SIZE);
        if (recv <= 0) {
            std::cerr << "Failed to read bytes from client socket connection" << std::endl;
            close(client_sock);
        }
        // Add the temporary buffer data to the client's buffer
        client_buff[client_sock] += std::string(buffer, recv);
        delete[] buffer;
        
        // Check if we've received a complete request
        size_t header_end = client_buff[client_sock].find("\r\n\r\n");
        // If yes we parse it
        if (header_end != std::string::npos)
            parse_request(client_sock, client_buff[client_sock]);
    }

    void TcpServer::send_response(int client_sock, Response &resp) {
        // Convert the response to a transferable format
        std::string mess = resp;
        // Send the response
        ssize_t sent = write(client_sock, mess.c_str(), mess.size());
        if (sent == (ssize_t)mess.size())
            std::clog << "Server response sent to client" << std::endl;
        else
            std::cerr << "Error sending response to client" << std::endl;
        // Close connection and erase buffer
        // This is not the intended way to do things from HTTP/1.1,
        // but further pipeline implementations would be neccesary to
        // actually utilize one socket for multiple connections
        close(client_sock);
        client_buff.erase(client_sock);
    }

    void TcpServer::create_endpoint(const std::string& method, const std::string& path, std::function<void(Request&, Response&)> callback){
        routes[method][path] = std::pair<std::string, std::function<void(Request&, Response&)>>(path, callback);
    }

    TcpServer::TcpServer(std::string ip_addr, int port): ip_addr(ip_addr), port(port), sock_struct_len(sizeof(sock_struct)) {
        // IPv4 socket
        sock_struct.sin_family = AF_INET;
        // Set the port and IP address
        sock_struct.sin_port = htons(port);
        sock_struct.sin_addr.s_addr = inet_addr(ip_addr.c_str());

        if (start_server() != 0)
            std::cerr << "Failed to start server with port: " << port << std::endl;

        // Create the kernel queue
        kq = kqueue();
        if (kq == -1)
            std::cerr << "Failed to create kqueue" << std::endl;
        
        // Get the current flags for the socket
        int flags = fcntl(sock, F_GETFL, 0);
        // Set the socket to non-blocking mode
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    }

    TcpServer::~TcpServer() {
        close(sock);
        exit(0);
    }

    void TcpServer::start_listen() {
        // Listening up to 20 connections
        if (listen(sock, 20) < 0)
            std::cerr << "Socket listen failed" << std::endl;
        
        // Create a kernel event for the socket change
        struct kevent event;

        // Configure the event for the socket
        // EVFILT_READ checks for available data to read (filter for the event)
        // EV_ADD adds the event to the kqueue (action flag for the kqueue)
        // other parameters unused
        EV_SET(&event, sock, EVFILT_READ, EV_ADD, 0, 0, NULL);

        // Register the event with the kqueue
        // Only register one event, not waiting for event triggers, no timeout
        if (kevent(kq, &event, 1, NULL, 0, NULL) == -1)
            std::cerr << "Failed to add socket to kqueue" << std::endl;
            
        std::clog << "Listening on address: " << inet_ntoa(sock_struct.sin_addr) << " port: " << ntohs(sock_struct.sin_port) << std::endl;
    }

    void TcpServer::parse_request(int client_sock, std::string req_text) {
        Request req;
        std::istringstream ss(req_text);
        std::string line;

        // Parse first line (ex.:, "GET / HTTP/1.1")
        std::getline(ss, line);
        std::istringstream firstLine(line);
        firstLine >> req.method >> req.path >> req.version;

        // Parse query path (if exists)
        size_t query_pos = req.path.find('?');
        if (query_pos != std::string::npos) {
            std::string query_str = req.path.substr(query_pos + 1);

            // Update path to be the actual path (query not included)
            req.path = req.path.substr(0, query_pos);

            // Split the query string
            for (std::string &pair: httpServer::Utils::split_str(query_str, "&")) {
                std::cout << pair << std::endl;
                size_t eq_pos = pair.find('=');
                std::string key = httpServer::Utils::url_decode(pair.substr(0, eq_pos));
                std::string value = httpServer::Utils::url_decode(pair.substr(eq_pos + 1));
                req.query[key] = value;
            }
        }

        // Parse headers (until empty line)
        while (std::getline(ss, line) && line != "\r\n") {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string val = line.substr(colon + 2);

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

        // Parse form data (if exists)
        if (req.headers.find("Content-Type") != req.headers.end()) {
            if (req.headers["Content-Type"] == "application/x-www-form-urlencoded") {
                // Split the query string
                for (std::string &pair: httpServer::Utils::split_str(req.body, "&")) {
                    size_t eq_pos = pair.find('=');
                    std::string key = httpServer::Utils::url_decode(pair.substr(0, eq_pos));
                    std::string value = httpServer::Utils::url_decode(pair.substr(eq_pos + 1));
                    req.query[key] = value;
                }
            }
        }

        Response resp;
        
        // Send response if route exists
        if (routes.find(req.method) != routes.end()) {
            if (routes[req.method].find(req.path) != routes[req.method].end()) {
                routes[req.method][req.path].second(req, resp);
                send_response(client_sock, resp);
            }
        }
    }

    void TcpServer::handler() {
        start_listen();

        // Kernel events array
        struct kevent events[TcpServer::MAX_EVENTS];
        while (true) {
            int ev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
            if (ev == -1)
                std::cerr << "kevent error" << std::endl;
            for (int i = 0; i < ev; i++) {
                int fd = events[i].ident; // file descriptor
                // New connection
                if (fd == sock)
                    accept_connection();
                // Data available from client
                else
                    handle_data(fd);
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