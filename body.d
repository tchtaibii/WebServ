
#pragma once
#include "Socket/socket.hpp"
#include <fcntl.h>
#include "requestParsing/checkRequest.hpp"
#define READ_N 1024
namespace ws
{
    std::map<int, server> ft_fds(std::vector<server> &servers)
    {
        std::map<int, server> fds;
        for (size_t i = 0; i < servers.size(); i++)
            fds.insert(std::make_pair(servers[i].getSocket(), servers[i]));
        return fds;
    }
    void connection_loop(std::vector<server> &servers)
    {
        std::map<int, server> fds_servers = ft_fds(servers);
        std::vector<int> fds;
        std::string tmp_body = "";
        std::vector<int> clients;
        HttpRequest req;
        fd_set readfds;
        fd_set writefds;
        int max = 0;
        int new_socket;
        FD_ZERO(&readfd