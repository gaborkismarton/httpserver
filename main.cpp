// https://itsfuad.medium.com/understanding-http-at-a-low-level-a-developers-guide-with-c-213728d6c41d
// https://medium.com/@ryan_forrester_/understanding-is-open-in-c-a-comprehensive-guide-a8e8f6b731ef
// https://www.udacity.com/blog/2021/05/how-to-read-from-a-file-in-cpp.html

#include "include/tcpServer.h"

int main() {
    httpServer::TcpServer server = httpServer::TcpServer("127.0.0.1", 8080);
    server.get("/", [](httpServer::Request &req, httpServer::Response &res) {
        res.html("index.html");
    });
    server.get("/test", [](httpServer::Request &req, httpServer::Response &res) {
        res.html("test.html");
    });
    server.start_listen();
    return 0;
}