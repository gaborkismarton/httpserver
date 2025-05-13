#include "include/utils.h"

namespace httpServer {
    // URL decoding function
    std::string Utils::url_decode(const std::string url) {
        std::string ret = "";
        for (size_t i = 0; i < url.size(); i++) {
            if (url[i] == '+') ret += ' ';
            // After % a hex value should be converted to a character
            else if (url[i] == '%' && i < (url.size() - 2)) {
                ret += (char)std::stoul(url.substr(i + 1, 2), nullptr, 16);;
                i += 2;
            }
            else ret += url[i];
        }
        return ret;
    }

    // String splitting function
    std::vector<std::string> Utils::split_str(const std::string str, const std::string delim) {
        size_t pos_start = 0, pos_end;
        std::string token;
        std::vector<std::string> ret;

        while ((pos_end = str.find(delim, pos_start)) != std::string::npos) {
            token = str.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim.length();
            ret.push_back(token);
        }

        ret.push_back(str.substr(pos_start));
        return ret;
    }
}