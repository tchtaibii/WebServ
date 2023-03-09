#pragma once
#include <iostream>
#include <string>
#include <map>
#include <sstream>

namespace ws
{
    // struct of http request
    struct HttpRequest
    {
        std::string method;
        std::string path;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
    };
    // Parse the HTTP request string into an HttpRequest struct
    HttpRequest parse_http_request(const char *request_str)
    {
        std::string tmp(request_str);
        size_t pos = tmp.find("\r\n\r\n");
        HttpRequest req;
        std::string header_tmp = tmp.substr(0, pos);
        std::string body_tmp = tmp.substr(pos + 4);
        // Split the request string into lines
        std::stringstream ss(header_tmp);
        std::string line;
        getline(ss, line); // First line is the request line
        std::stringstream req_line_ss(line);
        req_line_ss >> req.method >> req.path >> req.version;
        // Parse headers
        while (getline(ss, line))
        {
            if (line.empty())
                break; // End of headers
            pos = line.find(":");
            if (pos == std::string::npos)
                continue; // Malformed header
            std::string header_name = line.substr(0, pos);
            std::string header_value = line.substr(pos + 2);
            req.headers.insert(make_pair(header_name, header_value));
        }
        if (req.method == "POST")
        {
            if (req.headers.count("Content-Length"))
            {
                // Read the body using the Content-Length header
                int content_length = std::stoi(req.headers["Content-Length"]);
                req.body = body_tmp.substr(0, content_length);
            }
            else if (req.headers.count("Transfer-Encoding") && req.headers["Transfer-Encoding"] == "chunked\r")
            {
                // Read the body using chunked encoding
                std::stringstream ss(body_tmp);
            }
        }
        return req;
    }
}