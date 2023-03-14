#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>
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
        bool end_;
        bool Boundary;
        std::string Boundary_token;
        HttpRequest() : chunked(false), deja(false), con(false), end_(false), Boundary(false) {}
    };
    // Parse the HTTP request string into an HttpRequest struct
    bool isZero(const std::string &httpRequest)
    {
        size_t pos = httpRequest.find("\r\n0\r\n\r\n");
        if (pos != std::string::npos)
        {
            // std::cout << "dkhlaaaat f zero" << std::endl;
            return 1;
        }
        return 0;
    }
    size_t countSubstring(const std::string &str, const std::string &sub)
    {
        size_t count = 0;
        size_t pos = str.find(sub);

        while (pos != std::string::npos)
        {
            count++;
            pos = str.find(sub, pos + sub.length());
        }
        return count;
    }
    std::map<std::string, std::string> boundaryParsing(std::string body, HttpRequest &req)
    {
        std::map<std::string, std::string> boundary_files;
        size_t count = countSubstring(body, "----------------------------" + req.Boundary_token);
        for (size_t i = 0; i < count - 1; i++)
        {
            size_t pos = body.find("filename=");
            std::string fileName = body.substr(pos + 10);
            fileName = fileName.substr(0, fileName.find("\"\r\n"));
            body = body.substr(body.find("\r\n\r\n") + 2);
            std::string tmp = body.substr(2, body.find("----------------------------" + req.Boundary_token) - 4);
            body = body.substr(body.find(tmp) + tmp.length());
            boundary_files.insert(std::make_pair(fileName, tmp));
        }
        return boundary_files;
    }
    void httpRequestInit(HttpRequest &req)
    {
        req.method = "";
        req.path = "";
        req.version = "";
        req.headers.clear();
        req.body = "";
        req.Boundary_token = "";
        req.chunked = 0;
        req.deja = 0;
        req.con = 0;
        req.end_ = 0;
        req.Boundary = 0;
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
            {
                req.body = tmp.substr(pos + 2);
                try
                {
                    if (req.headers["Content-Type"].find("boundary=--------------------------"))
                    {
                        req.Boundary_token = req.headers["Content-Type"].substr(req.headers["Content-Type"].find("boundary=") + 9 + 26, 24);
                        std::cout << "|" << req.Boundary_token << "|" << std::endl;
                        req.Boundary = true;
                    }
                }
                catch (...)
                {
                    std::cerr << "" << '\n';
                }
            }
            // std::cout << "hadddi salaat" << std::endl;

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
        srand(time(NULL));                                                                               // seed the random number generator with the current time
        std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.$@*"; // the characters to choose from
        std::string result = "";
        for (int i = 0; i < length; i++)
        {
            int randomIndex = rand() % characters.length(); // generate a random index
            result += characters[randomIndex];              // add the random character to the result string
        }
        return result;
    }
    std::string remove_zero_chunked(std::string chunked_message)
    {
        size_t pos1 = chunked_message.find("\r\n");
        if (pos1 == std::string::npos)
            return chunked_message;
        std::string tmp = chunked_message.substr(pos1 + 2);
        size_t pos = tmp.find("0\r\n");
        if (pos == std::string::npos)
            return chunked_message;
        chunked_message = chunked_message.replace(pos1, 5, "");
        return chunked_message;
    }
    std::string remove_chunk_headers(std::string chunked_message)
    {
        while (1)
        {
            size_t pos1 = chunked_message.find("\r\n");
            if (pos1 == std::string::npos)
                break;
            std::string tmp = chunked_message.substr(pos1 + 2);
            size_t pos = tmp.find("\r\n");
            if (pos == std::string::npos)
                break;
            tmp = tmp.substr(0, pos);
            if (isHexadecimal(tmp))
            {
                tmp = "\r\n" + tmp + "\r\n";
                chunked_message = chunked_message.replace(pos1, tmp.length(), "");
            }
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
                if ((lenght_body + 2 == body.length()))
                {
                    req.body = body.substr(2);
                    if (req.Boundary)
                    {
                        req.deja = false;
                        std::map<std::string, std::string> boundary_files = boundaryParsing(req.body, req);
                        for (std::map<std::string, std::string>::iterator it = boundary_files.begin(); it != boundary_files.end(); it++)
                        {
                            std::string tmp = it->first + "tmp";
                            std::ofstream file(tmp);
                            if (file.is_open())
                            {
                                file << it->second;
                                file.close();
                            }
                        }
                        httpRequestInit(req);
                        return true;
                    }
                    else
                    {
                        req.deja = false;
                        std::ofstream file(randomString(18));
                        if (file.is_open())
                        {
                            file << req.body;
                            file.close();
                        }
                        httpRequestInit(req);
                        return true;
                    }
                }
                else
                    return false;
            }
            else if (!b.empty() && "chunked\r" == b) // chunked
            {
                req.chunked = true;
                if (the_end)
                {
                    
                    if (req.Boundary)
                    {
                        req.deja = false;
                        req.body = body.substr(2);
                        std::map<std::string, std::string> boundary_files = boundaryParsing(req.body, req);
                        for (std::map<std::string, std::string>::iterator it = boundary_files.begin(); it != boundary_files.end(); it++)
                        {
                            it->second = remove_chunk_headers(it->second);
                            it->second = remove_zero_chunked(it->second);
                            std::string tmp = it->first + "tmp";
                            std::ofstream file(tmp);
                            if (file.is_open())
                            {
                                file << it->second;
                                file.close();
                            }
                        }
                        httpRequestInit(req);
                        return true;
                    }
                    body = remove_chunk_headers(body);
                    body = remove_zero_chunked(body);
                    req.body = body.substr(0, body.length() - 2);
                    std::ofstream file(randomString(18));
                    if (file.is_open())
                    {
                        file << req.body;
                        file.close();
                    }
                    httpRequestInit(req);
                    return true;
                }
                return false;
            }
        }
        return false;
    }
}
