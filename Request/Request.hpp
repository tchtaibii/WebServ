#pragma once
#include "tools.hpp"
namespace ws
{
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
        bool headers_complet;
        HttpRequest() : chunked(false), deja(false), con(false), end_(false), Boundary(false), headers_complet(false) {}
    };
}
#include "boundary.hpp"
#include "Chunked.hpp"
namespace ws
{
    void httpRequestInit(HttpRequest &req, bool all)
    {
        if (all)
        {
            req.method.clear();
            req.path.clear();
            req.version.clear();
            req.headers.clear();
        }
        req.body.clear();
        req.Boundary_token.clear();
        req.chunked = 0;
        req.deja = 0;
        req.con = 0;
        req.end_ = 0;
        req.Boundary = 0;
        req.headers_complet = false;
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
                    size_t p = body.find("----------------------------" + req.Boundary_token + "--");
                    if (req.Boundary && p != std::string::npos)
                    {
                        std::map<std::string, std::string> boundary_files = boundaryParsing(body, req);
                        for (std::map<std::string, std::string>::iterator it = boundary_files.begin(); it != boundary_files.end(); it++)
                        {
                            std::string tmp = it->first;
                            std::ofstream file("./Assets/uploads/" + tmp);
                            if (file.is_open())
                            {
                                file << it->second;
                                file.close();
                            }
                            httpRequestInit(req, 0);
                        }
                        return true;
                    }
                    else
                    {
                        req.body = body.substr(2);
                        req.deja = false;
                        std::string extension = req.headers["Content-Type"].substr(req.headers["Content-Type"].find("/") + 1, req.headers["Content-Type"].find("\r"));
                        std::ofstream file("./Assets/uploads/" + getCurrentDateTime() + "." + extension);
                        if (file.is_open())
                        {
                            file << req.body;
                            file.close();
                        }
                        httpRequestInit(req, 0);
                        return true;
                    }
                }
                else
                    return false;
            }
            else if (req.chunked) // chunked
            {
                if (the_end)
                {
                    verifyChunk(req.body);
                    std::string extension = req.headers["Content-Type"].substr(req.headers["Content-Type"].find("/") + 1, req.headers["Content-Type"].find("\r"));
                    std::ofstream file("./Assets/uploads/" + getCurrentDateTime() + "." + extension);
                    if (file.is_open())
                    {
                        file << req.body;
                        file.close();
                    }
                    httpRequestInit(req, 0);
                    return true;
                }
                return false;
            }
        }
        return false;
    }
    HttpRequest parse_http_request(std::string tmp, HttpRequest &req, std::string &request_im)
    {
        if (tmp.find("\r\n\r\n") == std::string::npos && !req.headers_complet)
        {
            request_im += tmp;
        }
        else if (!req.deja)
        {
            request_im += tmp.substr(0, tmp.length());
            tmp = request_im.substr(0, request_im.length());
            req.headers_complet = true;
            size_t pos = tmp.find("\r\n\r\n");
            std::string header_tmp = tmp.substr(0, pos);
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
                        req.Boundary = true;
                    }
                }
                catch (...)
                {
                    std::cerr << "" << '\n';
                }
                std::string b = req.headers["Transfer-Encoding"];
                if (!b.empty() && "chunked\r" == b)
                {
                    req.chunked = true;
                    std::string tmp_tmp = req.body;
                    req.body = "";
                    chunked_uncoding(tmp_tmp, req);
                    tmp_tmp.clear();
                }
            }
            req.deja = true;
        }
        return req;
    }
}
