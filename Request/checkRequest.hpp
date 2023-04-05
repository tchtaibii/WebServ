#pragma once
#include "Request.hpp"
namespace ws
{
    int is_req_well_formed(HttpRequest &req)
    {
        std::map<std::string, std::string> hed(req.headers.begin(), req.headers.end());
        std::string a = hed["Transfer-Encoding"];
        std::string C = hed["Content-Length"];
        if (!a.empty() && a != "chunked")
            return 501;
        if (req.path.length() > 2048)
            return 414;
        if (a.empty() && C.empty() && req.method == "POST")
            return 400;
        for (int i = 0; req.path[i]; i++)
        {
            if ((isalnum(req.path[i])) || (req.path[i] == 33) || (req.path[i] >= 35 && req.path[i] <= 47) || (req.path[i] <= 60 && req.path[i] >= 57) || (req.path[i] == 61) || (req.path[i] >= 63 && req.path[i] <= 64) || (req.path[i] == 95) || (req.path[i] == 126))
                i++;
            else
                return 400;
        }
        return 0;
    }
}
