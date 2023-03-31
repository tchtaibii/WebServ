#pragma once
#include "tools.hpp"
#include "Request.hpp"
namespace ws
{
    std::map<std::string, std::string> boundaryParsing(std::string body, HttpRequest &req)
    {
        std::map<std::string, std::string> boundary_files;
        size_t count = countSubstring(body, "-----------------------------" + req.Boundary_token) - 1;
        for (size_t i = 0; i < count; i++)
        {
            if (i > 0)
            {
                if (!body.empty())
                    if (body[0] == '\r' && body[1] == '\n')
                    {
                        body.erase(0, 2);
                    }
            }
            std::string fileName;
            size_t pos = body.find("Content-Disposition");
            size_t n = body.find("\r\n", pos);
                // std::cerr << "**************" << body << "****************" << std::endl;
            if (n == std::string::npos)
                boundary_files.insert(std::make_pair("", ""));
            else
            {
                std::string tmp_body = body.substr(pos, n);
                pos = tmp_body.find("filename=");
                if (pos != std::string::npos)
                {
                    fileName = tmp_body.substr(pos + 10);
                    if (fileName[pos + 10] != '\"')
                        fileName = fileName.substr(0, fileName.find("\"\r\n"));
                    else
                        fileName = "";
                }
                else
                {
                    pos = tmp_body.find("name=");
                    if (pos != std::string::npos)
                    {
                        fileName = tmp_body.substr(pos, n);
                        if (fileName[pos + 6] != '\"')
                        {
                            fileName = fileName.substr(6, fileName.find("\"") - 1) + ".input";
                        }
                        else
                        {
                            std::stringstream ss;
                            ss << i;
                            std::string str = ss.str();
                            if (str != "0")
                                fileName = getCurrentDateTime() + "(" + str + ")" + ".input";
                            else
                                fileName = getCurrentDateTime() + ".input";
                        }
                    }
                }
                body = body.substr(body.find("\r\n\r\n") + 2);
                std::string tmp = body.substr(2, body.find("-----------------------------" + req.Boundary_token) - 4);
                body = body.substr(body.find(tmp) + tmp.length());
                if (tmp.find("-----------------------------" + req.Boundary_token) != std::string::npos)
                    tmp.clear();
                boundary_files.insert(std::make_pair(fileName, tmp));
            }
        }
        return boundary_files;
    }
}