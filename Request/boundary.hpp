#pragma once
#include "tools.hpp"
#include "Request.hpp"
namespace ws
{
    std::map<std::string, std::string> boundaryParsing(std::string body, HttpRequest &req)
    {
        std::map<std::string, std::string> boundary_files;
        size_t count = countSubstring(body, "----------------------------" + req.Boundary_token);
        for (size_t i = 0; i < count - 1; i++)
        {
            size_t pos = body.find("filename=");
            std::string fileName;
            if (pos != std::string::npos)
                fileName = body.substr(pos + 10);
            else
                fileName = req.Boundary_token + " (" + randomString(1) + ")";
            fileName = fileName.substr(0, fileName.find("\"\r\n"));
            body = body.substr(body.find("\r\n\r\n") + 2);
            std::string tmp = body.substr(2, body.find("----------------------------" + req.Boundary_token) - 4);
            body = body.substr(body.find(tmp) + tmp.length());
            boundary_files.insert(std::make_pair(fileName, tmp));
        }
        return boundary_files;
    }
}