#pragma once
#include <iostream>
#include <string>
#include <map>
#include <sstream>

namespace ws{
    // struct of http request
    struct HttpRequest {
        std::string method;
        std::string path;
        std::string version;
        std::map<std::string, std::string> headers;
    };
    // Parse the HTTP request string into an HttpRequest struct
    HttpRequest parse_http_request(const char *request_str)
    {
        HttpRequest req;
        // Split the request string into lines
        std::stringstream ss(request_str);
        std::string line;
        getline(ss, line);  // First line is the request line
        std::stringstream req_line_ss(line);
        req_line_ss >> req.method >> req.path >> req.version;
        // Parse headers
        while (getline(ss, line))
        {
            if (line.empty()) break;  // End of headers
            size_t pos = line.find(":");
            if (pos == std::string::npos) continue;  // Malformed header
            std::string header_name = line.substr(0, pos);
            std::string header_value = line.substr(pos + 1);
            req.headers.insert(make_pair(header_name, header_value));
        }
        return req;
    }
}