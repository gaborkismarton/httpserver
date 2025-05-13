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

    Response& Response::html_file(const std::string& path){
        header("Content-Type", "text/html");
        std::ifstream file(path);
        std::string str;
        if(file.is_open()) {
            while (std::getline(file, str))
                *this << str;
        }
        else
            status_code(404) << httpserver::reason(404);
        return *this;
    }

    Response& Response::html_str(const std::string& html_code) {
        header("Content-Type", "text/html");
        *this << html_code;
        return *this;
    }

    Response& Response::json(const std::string& data) {
        header("Content-Type", "application/json");
        body = data;
        return *this;
    }

    Response::operator std::string() const {
        std::ostringstream ss;
        ss << "HTTP/1.1 ";
        ss << status << headers << "\r\n\r\n" << body << "\r\n";
        return ss.str();
    }
}