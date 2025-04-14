#include "include/response.h"

namespace httpServer {
    Response::Response() {
        status = "200 OK\r\n";
    }
    
    Response& Response::operator<<(const std::string &str) {
        body += str;
        return *this;
    }

    Response& Response::header(const std::string& key, const std::string& value) {
        headers += key + ": " + value + "\r\n";
        return *this;
    }

    Response& Response::status_code(const int code) {
        status = std::to_string(code) + httpserver::reason(code);
        return *this;
    }

    Response& Response::html(const std::string& path){
        header("Content-Type", "text/html");
        std::ifstream html_file(path);
        std::string str;
        if(html_file.is_open()) {
            while (std::getline(html_file, str))
                *this << str;
        }
        else
            status_code(404) << httpserver::reason(404);
        return *this;
    }

    Response& Response::json(const std::string& data) {
        header("Content-Type", "application/json");
        body = data;
        return *this;
    }
}