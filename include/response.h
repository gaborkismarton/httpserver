#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <fstream>
#include "include/statusCodes.h"

namespace httpServer {
    class Response {
    public:
        std::string status;
        std::string body;
        std::string headers;

        Response();
        Response& operator<<(const std::string &str);
        Response& header(const std::string& key, const std::string& value);
        Response& status_code(const int code);
        Response& html(const std::string& path);
        Response& json(const std::string& data);
    };
}

#endif // RESPONSE_H