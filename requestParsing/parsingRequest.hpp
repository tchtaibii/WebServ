#pragma once
#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>

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
        bool chunked;
        bool deja;
        bool con;
        HttpRequest() : chunked(false), deja(false), con(false) {}
    };
    // Parse the HTTP request string into an HttpRequest struct
    bool isZero(const std::string &httpRequest)
    {
        size_t pos = httpRequest.find("\r\n0\r\n\r\n");
        if (pos != std::string::npos)
            return 1;
        return 0;
    }
    HttpRequest parse_http_request(std::string tmp, HttpRequest &req)
    {
        // std::string tmp(request_str);
        if (!req.deja)
        {
            size_t pos = tmp.find("\r\n\r\n");
            std::string header_tmp = tmp.substr(0, pos);
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
                size_t tmp_pos = line.find(":");
                if (tmp_pos == std::string::npos)
                    continue; // Malformed header
                std::string header_name = line.substr(0, tmp_pos);
                std::string header_value = line.substr(tmp_pos + 2);
                req.headers.insert(make_pair(header_name, header_value));
            }
            if (req.method == "POST")
                req.body = tmp.substr(pos + 2);
            req.deja = true;
        }
        return req;
    }
    bool isHexadecimal(std::string str)
    {
        // Iterate over each character in the string
        for (size_t i = 0; i < str.length(); i++)
        {
            char c = toupper(str[i]);
            if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
            {
                // Character is a valid hexadecimal digit
                continue;
            }
            else
            {
                // Character is not a valid hexadecimal digit
                return false;
            }
        }
        return true;
    }
    std::string randomString(int length)
    {
        srand(time(NULL));                                                                    // seed the random number generator with the current time
        std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.$@*"; // the characters to choose from
        std::string result = "";
        for (int i = 0; i < length; i++)
        {
            int randomIndex = rand() % characters.length(); // generate a random index
            result += characters[randomIndex];              // add the random character to the result string
        }
        return result;
    }

    std::string remove_chunk_headers(std::string chunked_message)
    {
        size_t pos1 = chunked_message.find("\r\n");
        if (pos1 == std::string::npos)
            return chunked_message;
        std::string tmp = chunked_message.substr(pos1 + 2);
        size_t pos = tmp.find("\r\n");
        if (pos == std::string::npos)
            return chunked_message;
        tmp = tmp.substr(0, pos);
        if (isHexadecimal(tmp))
        {
            tmp = "\r\n" + tmp + "\r\n";
            chunked_message = chunked_message.replace(pos1, tmp.length(), "");
        }
        return chunked_message;
    }
    bool bodyParsing(HttpRequest &req, std::string &body, bool the_end)
    {
        if (req.method == "POST")
        {
            std::string a = req.headers["Content-Length"];
            std::string b = req.headers["Transfer-Encoding"];
            if (!a.empty())
            {
                size_t lenght_body = atoi(a.c_str());
                if (lenght_body == body.length())
                {
                    req.body = body;
                    req.deja = false;
                    return true;
                }
                else
                    return false;
            }
            else if (!b.empty() && "chunked\r" == b) // chunked
            {
                req.chunked = true;
                body = remove_chunk_headers(body);
                if (the_end)
                {
                    req.body = body;
                    req.deja = false;
                    the_end = 0;
                    std::ofstream file(randomString(18)); // create an output file stream
                    if (file.is_open())
                    {                     // check if the file is open
                        file << req.body; // write the string to the file
                        file.close();     // close the file
                    }
                    req.body = "";
                    // std::cout << req.body << std::endl;
                    return true;
                }
                return false;
            }
        }
        return false;
    }
}
