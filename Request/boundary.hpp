#pragma once
#include "tools.hpp"
#include "Request.hpp"
namespace ws
{
    std::map<std::string, std::pair<bool, std::string> > boundaryParsing(std::string body, HttpRequest &req)
    {
        std::map<std::string, std::pair<bool, std::string> > boundary_files;
        size_t count = countSubstring(body,req.Boundary_token) - 1;
        size_t dash = body.substr(0, body.find(req.Boundary_token)).substr(2).length();
        for (size_t i = 0; i < count; i++)
        {
            bool input_;
            if (i > 0)
            {
                if (!body.empty())
                    if (body[0] == '\r' && body[1] == '\n')
                        body.erase(0, 2);
            }
            std::string fileName;
            size_t pos = body.find("Content-Disposition");
            size_t n = body.find("\r\n", pos);
            if (n == std::string::npos)
                boundary_files.insert(std::make_pair("", std::make_pair(false, "")));
            else
            {
                std::string tmp_body = body.substr(pos, n);
                pos = tmp_body.find("filename=");
                if (pos != std::string::npos)
                {
                    input_ = false;
                    fileName = tmp_body.substr(pos + 10);
                    if (fileName[pos + 10] != '\"')
                        fileName = fileName.substr(0, fileName.find("\"\r\n"));
                    else
                        fileName = "";
                }
                else
                {
                    input_ = true;
                    pos = tmp_body.find("name=");
                    if (pos != std::string::npos)
                    {
                        fileName = tmp_body.substr(pos, n);
                        if (fileName[6] != '\"')
                        {
                            size_t i = 6;
                            for (; fileName[i]; i++)
                                if(fileName[i] == '"')
                                    break;
                            i -= 6;
                            fileName = fileName.substr(6, i);
                        }
                        else
                            fileName = "";
                    }
                }
                body = body.substr(body.find("\r\n\r\n") + 2);
                std::string tmp = body.substr(2, body.find(dashes(dash) + req.Boundary_token) - 4);
                body = body.substr(body.find(tmp) + tmp.length());
                if (tmp.find(dashes(dash) + req.Boundary_token) != std::string::npos)
                    tmp.clear();
                boundary_files.insert(std::make_pair(fileName, std::make_pair(input_, tmp)));
                fileName.clear();
            }
        }
        return boundary_files;
    }
}