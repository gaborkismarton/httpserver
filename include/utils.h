#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>

namespace httpServer {
    class Utils {
    public:
        static std::string url_decode(const std::string url);
        static std::vector<std::string> split_str(const std::string str, const std::string delim);
    };
}

#endif // UTILS_H