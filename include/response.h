#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <sstream>
#include <fstream>
#include "include/statusCodes.h"

namespace httpServer {
    class Response {
    private:
        std::string status;
        std::string body;
        std::string headers;
    public:
        Response();
        Response& operator<<(const std::string &str);
        Response& header(const std::string& key, const std::string& value);
        Response& status_code(const int code);
        Response& html_file(const std::string& path);
        Response& html_str(const std::string& html_code);
        Response& json(const std::string& data);
        operator std::string() const;
    };
}

#endif // RESPONSE_H