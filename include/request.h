#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <map>

namespace httpServer {
    class Request {
    public:
        std::string method;
        std::string path;
        std::string version;
        std::string body; 
        std::map<std::string, std::string> headers;
        std::map<std::string, std::string> query;
        std::map<std::string, std::string> form;
    };
}

#endif // REQUEST_H