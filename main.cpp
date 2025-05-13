#include "include/tcpServer.h"

int main() {
    httpServer::TcpServer server = httpServer::TcpServer("127.0.0.1", 8081);
    
    // Create the endpoints
    server.get("/", [](httpServer::Request &req, httpServer::Response &res) {
        res.html_file("index.html");
    });
    server.get("/test", [](httpServer::Request &req, httpServer::Response &res) {
        res.html_file("test.html");
    });
    server.get("/form", [](httpServer::Request &req, httpServer::Response &res) {
        res.html_file("form.html");
    });
    server.post("/form", [](httpServer::Request &req, httpServer::Response &res) {
        std::ostringstream ss;
        ss << "<!DOCTYPE html><html><body><script>alert(\"Hello ";
        ss << req.query["fname"] << " " << req.query["lname"];
        ss << "\")</script></body></html>";
        res.html_str(ss.str());
    });

    server.handler();
    return 0;
}