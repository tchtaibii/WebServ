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
        std::string query;
        bool chunked;
        bool deja;
        bool con;
        bool end_;
        bool Boundary;
        std::string Boundary_token;
        bool headers_complet;
        std::string upload;
        int chunked_c;
        std::map<std::string, std::string> cookies;
        std::string session;
        bool NoUpload;
        std::string port;
        HttpRequest() : chunked(false), deja(false), con(false), end_(false), Boundary(false), headers_complet(false), chunked_c(0), NoUpload(0) {}
    };
}
#include "boundary.hpp"
#include "Chunked.hpp"
#include "../Config/parse.hpp"
#include "../Session_management/Session.hpp"
#include <iostream>
namespace ws
{
    void httpRequestInit(HttpRequest &req, bool all)
    {
        if (all)
        {
            req.session.clear();
            req.method.clear();
            req.cookies.clear();
            req.path.clear();
            req.version.clear();
            req.headers.clear();
            req.upload.clear();
            req.NoUpload = 0;
            req.body.clear();
        }
        req.Boundary_token.clear();
        req.chunked = 0;
        req.deja = 0;
        req.con = 0;
        req.end_ = 0;
        req.Boundary = 0;
        req.chunked_c = 0;
        req.headers_complet = false;
    }

    bool bodyParsing(HttpRequest &req, std::string &body, bool the_end, server &server_)
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
                        if (!req.NoUpload)
                        {
                            for (std::map<std::string, std::string>::iterator it = boundary_files.begin(); it != boundary_files.end(); it++)
                            {
                                if (it->second.size() != 0)
                                {
                                    std::string tmp = it->first;
                                    std::ofstream file(req.upload + "/" + tmp);
                                    if (file.is_open())
                                    {
                                        file << it->second;
                                        file.close();
                                    }
                                    req.body.clear();
                                    httpRequestInit(req, 0);
                                }
                            }
                        }
                        else
                            req.body = body;
                        return true;
                    }
                    else
                    {
                        req.body = body.substr(2);
                        req.deja = false;
                        if (!req.NoUpload)
                        {
                            std::string extension = req.headers["Content-Type"].substr(req.headers["Content-Type"].find("/") + 1, req.headers["Content-Type"].find("\r"));
                            std::ofstream file(req.upload + "/" + getCurrentDateTime() + "." + extension);
                            if (file.is_open())
                            {
                                file << req.body;
                                file.close();
                            }
                            req.body.clear();
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
                    if (atoi(server_.get_body_size().c_str()) < (int)req.body.length())
                    {
                        //     server_.setStatus(413);
                        //     return true;
                    }
                    req.body = req.body.substr(0, req.body.length() - 2);
                    if (!req.NoUpload)
                    {
                        std::string extension = req.headers["Content-Type"].substr(req.headers["Content-Type"].find("/") + 1, req.headers["Content-Type"].find("\r"));
                        std::ofstream file(req.upload + "/" + getCurrentDateTime() + "." + extension);
                        if (file.is_open())
                        {
                            file << req.body;
                            file.close();
                        }
                        req.body.clear();
                        httpRequestInit(req, 0);
                    }
                    return true;
                }
                return false;
            }
        }
        return false;
    }
    HttpRequest parse_http_request(std::string tmp, HttpRequest &req, std::string &request_im, server server_)
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
            size_t n;
            if ( (n = req.path.find('?')) != std::string::npos)
                req.query = req.path.substr(n + 1);
            std::cout << req.query << std::endl;
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
            if (req.headers.count("Cookie") > 0)
            {
                req.cookies = parseKeyValuePairs(req.headers["Cookie"].substr(0, req.headers["Cookie"].length() - 1));
                req.session = req.cookies["session_id"];
                std::fstream myfile("./Session_management/sessionIds", std::ios::in | std::ios::out | std::ios::app);
                std::string line;
                if (myfile.is_open())
                {
                    bool Token_d;
                    while (getline(myfile, line))
                    {
                        if (line.find(req.session) != std::string::npos)
                        {
                            Token_d = true;
                            break;
                        }
                        else
                            Token_d = false;
                    }
                    myfile.close();
                    if (!Token_d)
                    {

                        std::ofstream myfile2("./Session_management/sessionIds", std::ios::app);
                        if (myfile2.is_open())
                        {
                            myfile2 << "session_id=" + req.session + ";\n";
                            myfile2.close();
                        }
                    }
                }
                setCookies_onfile(req.cookies, req.session);
            }
            else
                req.session = generateSession();
            if (req.method == "POST")
            {
                location my_location = server_.locationChecker(req.path, server_.get_location())->second;
                if (!my_location.get_upload().empty())
                {
                    req.upload = my_location.get_root().substr(0, my_location.get_root().length() - 1) + my_location.get_upload();
                }
                else
                {
                    req.NoUpload = true;
                }
                req.body = tmp.substr(pos + 2);
                try
                {
                    if (req.headers["Content-Type"].find("boundary=--------------------------"))
                    {
                        std::string tmpb = req.headers["Content-Type"].substr(req.headers["Content-Type"].find("boundary=") + 36, 70);
                        for (size_t i = 0; tmpb[i]; i++)
                        {
                            if (tmpb[i] >= '0' && tmpb[i] <= '9')
                                req.Boundary_token.insert(i, 1, tmpb[i]);
                            else
                                break;
                        }
                        tmpb.clear();
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
                    req.body.clear();
                    chunked_uncoding(tmp_tmp, req);
                    tmp_tmp.clear();
                }
            }
            req.deja = true;
        }
        return req;
    }
}