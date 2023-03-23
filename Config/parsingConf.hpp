#pragma once
#include <iostream>
#include "parse.hpp"
#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>

std::pair<std::string, std::string> split(std::string s)
{
    std::string key, value;
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    key = s.substr(0, s.find("="));
    value = s.substr(s.find("=") + 1, s.length());
    return std::make_pair(key, value);
}

ws::server &server_parse(ws::server &s, const std::pair<std::string, std::string> &m,
                         std::map<std::string, std::string> &b)
{
    if (m.first == "port")
        s.set_port(m.second);
    else if (m.first == "host")
        s.set_host(m.second);
    else if (m.first == "server_name")
        s.set_server_name(m.second);
    else if (m.first == "error_page")
        s.set_error_page(m.second);
    else if (m.first == "body_size_limit")
        s.set_body_size(m.second);
    else if (m.first == "cgi")
    {
        b.insert(std::make_pair(m.second.substr(0, m.second.find("/")),
                                m.second.substr(m.second.find("/"), m.second.length())));
        s.set_cgi(b);
    }
    return s;
}

ws::location &location_parse(std::pair<std::string, std::string> &m, ws::location &l, std::string &PWD)
{
    size_t os = m.second.find("${PWD}");
    if (os != std::string::npos)
        m.second = m.second.replace(os, 6, PWD);
    if (m.first == "method")
    {
        size_t pos;
        std::vector<std::string> h;

        pos = m.second.find("GET");
        if (pos != std::string::npos)
            h.push_back("GET");
        pos = m.second.find("POST");
        if (pos != std::string::npos)
            h.push_back("POST");
        pos = m.second.find("DELETE");
        if (pos != std::string::npos)
            h.push_back("DELETE");
        l.set_method(h);
    }
    else if (m.first == "root")
        l.set_root(m.second);
    else if (m.first == "autoindex")
        l.set_autoindex(m.second);
    else if (m.first == "default")
        l.set_default(m.second);
    else if (m.first == "upload")
        l.set_upload(m.second);
    else if (m.first == "redirect")
    {
        std::map<std::string, std::string> p;
        std::string first = m.second.substr(0, m.second.find("http"));
        std::string second = m.second.substr(m.second.find("http"), m.second.length());
        p.insert(std::make_pair(first, second));
        l.set_redirect(p);
    }
    return l;
}

std::vector<ws::server> ConfingParsing(char **av, std::string &PWD)
{
    std::vector<ws::server> _s;
    ws::server s;
    std::ifstream ifs(av[1]);
    std::string line;
    std::pair<std::string, std::string> m;
    std::map<std::string, std::string> b;
    std::map<std::string, ws::location> _l;
    ws::location l;
    std::string temp;
    size_t pos;

    s.flg = false;
    while (std::getline(ifs, line))
    {
        pos = line.find("server");
        if (pos != std::string::npos && !s.flg)
        {
            s.flg = true;
            while (std::getline(ifs, line) && s.flg)
            {
                if (!line.empty())
                    m = split(line);
                if (m.first == "}")
                    s.flg = false;
                else if (m.first != "location")
                    s = server_parse(s, m, b);
                else if (m.first == "location")
                {
                    if (line.find("["))
                        l.flg = true;
                    temp = m.second.substr(0, m.second.find("["));
                    while (std::getline(ifs, line) && l.flg)
                    {
                        m = split(line);
                        l = location_parse(m, l, PWD);
                        if (m.first == "]")
                        {
                            l.flg = false;
                            break;
                        }
                    }
                    _l.insert(std::make_pair(temp, l));
                }
            }
            if (!b.empty())
            {
                s.set_cgi(b);
                b.clear();
            }
            if (!_l.empty())
            {
                s.set_location(_l);
                _l.clear();
            }
            _s.push_back(s);
        }
    }
    return _s;
}