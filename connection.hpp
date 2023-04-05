#pragma once
#include "Socket/socket.hpp"
#include <fcntl.h>
#include "Request/checkRequest.hpp"
#define READ_N 2048
namespace ws
{
    void change_socket(std::map<int, server> &fds_server, int fileD, int newSocket)
    {
        server tmp = fds_server[fileD];
        fds_server.insert(std::make_pair(newSocket, tmp));
        fds_server[newSocket].setSocket(newSocket);
    }

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
        std::string tmp_body;
        std::string request_im;
        std::vector<int> clients;
        HttpRequest req;
        fd_set readfds;
        fd_set writefds;
        int tmp = 1;
        int max = 0;
        int new_socket;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        for (std::map<int, server>::iterator it = fds_servers.begin(); it != fds_servers.end(); it++)
        {
            fds.push_back(it->first);
            FD_SET(it->first, &readfds);
            FD_SET(it->first, &writefds);
            if (it->first > max)
                max = it->first;
        }
        while (1)
        {
            fd_set tmp_readfds = readfds;
            fd_set tmp_writefds = writefds;
            if (select(max + 1, &tmp_readfds, &tmp_writefds, NULL, NULL) < 0)
                throw std::runtime_error("Select error");
            for (int fileD = 0; fileD <= max; fileD++)
            {
                if (FD_ISSET(fileD, &tmp_readfds) || FD_ISSET(fileD, &tmp_writefds))
                {
                    if (std::count(fds.begin(), fds.end(), fileD))
                    {
                        new_socket = accept(fileD, NULL, NULL);
                        fcntl(new_socket, F_SETFL, O_NONBLOCK);
                        setsockopt(new_socket, SOL_SOCKET, SO_NOSIGPIPE, &tmp, sizeof(tmp));
                        FD_SET(new_socket, &readfds);
                        clients.push_back(new_socket);
                        change_socket(fds_servers, fileD, new_socket);
                        if (new_socket > max)
                            max = new_socket;
                        continue;
                    }
                    else
                    {
                        if (FD_ISSET(fileD, &tmp_readfds))
                        {
                            char buffer[READ_N] = {0};
                            int valread = recv(fileD, buffer, READ_N, 0);
                            if (valread < 0)
                            {
                                clients.erase(std::remove(clients.begin(), clients.end(), fileD), clients.end());
                                fds.erase(std::remove(fds.begin(), fds.end(), fileD), fds.end());
                                close(fileD);
                                FD_CLR(fileD, &readfds);
                                max = *std::max_element(clients.begin(), clients.end());
                                httpRequestInit(req, 0);
                            }
                            else if (valread > 0)
                            {
                                std::string request_str = std::string(buffer, valread);
                                if (!req.deja)
                                {
                                    req = parse_http_request(request_str, req, request_im, fds_servers[fileD]);
                                    if (!req.headers_complet)
                                        continue;
                                    fds_servers[fileD].set_req(req);
                                    fds_servers[fileD].is_req_well_formed();
                                    request_im.clear();
                                    tmp_body = req.body;
                                    req.deja = true;
                                    if (req.method != "POST")
                                    {
                                        httpRequestInit(req, 0);
                                        FD_SET(fileD, &writefds);
                                        FD_CLR(fileD, &readfds);
                                        continue;
                                    }
                                    if (fds_servers[fileD].get_status() && req.method == "POST" != 0 && req.headers["Content-Length"].empty() && req.headers["Transfer-Encoding"].empty())
                                    {
                                        fds_servers[fileD].set_req(req);
                                        httpRequestInit(req, 0);
                                        FD_SET(fileD, &writefds);
                                        FD_SET(fileD, &tmp_writefds);
                                        FD_CLR(fileD, &readfds);
                                        FD_CLR(fileD, &tmp_readfds);
                                        req.con = 0;
                                        continue;
                                    }
                                    if (req.method == "POST" && atoi(req.headers["Content-Length"].c_str()) + 2 == (int)req.body.length())
                                    {
                                        bodyParsing(req, tmp_body, 0, fds_servers[fileD]);
                                        httpRequestInit(req, 0);
                                        FD_SET(fileD, &writefds);
                                        FD_SET(fileD, &tmp_writefds);
                                        FD_CLR(fileD, &readfds);
                                        FD_CLR(fileD, &tmp_readfds);
                                        continue;
                                    }
                                }
                                else
                                {
                                    if (!req.con && req.method == "POST")
                                    {
                                        if (req.chunked)
                                        {
                                            req.end_ = isZero(request_str);
                                            chunked_uncoding(request_str, req);
                                            request_str.clear();
                                            req.con = bodyParsing(req, tmp_body, req.end_, fds_servers[fileD]);
                                            fds_servers[fileD].set_req(req);
                                        }
                                        else
                                        {
                                            tmp_body += request_str;
                                            request_str.clear();
                                            req.con = bodyParsing(req, tmp_body, 0, fds_servers[fileD]);
                                        }
                                        if (req.con)
                                        {
                                            fds_servers[fileD].set_req(req);
                                            httpRequestInit(req, 0);
                                            FD_SET(fileD, &writefds);
                                            FD_SET(fileD, &tmp_writefds);
                                            FD_CLR(fileD, &readfds);
                                            FD_CLR(fileD, &tmp_readfds);
                                            req.con = 0;
                                            continue;
                                        }
                                    }
                                }
                            }
                            else if (valread == 0)
                            {
                                httpRequestInit(req, 1);
                                clients.erase(std::remove(clients.begin(), clients.end(), fileD), clients.end());
                                fds.erase(std::remove(fds.begin(), fds.end(), fileD), fds.end());
                                close(fileD);
                                FD_CLR(fileD, &readfds);
                                max = *std::max_element(clients.begin(), clients.end());
                                continue;
                            }
                            else
                            {
                                request_im.clear();
                                httpRequestInit(req, 1);
                                throw std::runtime_error("Read error");
                            }
                        }
                        else if (FD_ISSET(fileD, &tmp_writefds))
                        {
                            if (!fds_servers[fileD].get_status())
                            {
                                httpRequestInit(req, 1);
                                fds_servers[fileD].checker();
                            }
                            fds_servers[fileD].response();
                            if (fds_servers[fileD].getDone())
                            {
                                FD_CLR(fileD, &writefds);
                                FD_CLR(fileD, &readfds);
                                FD_CLR(fileD, &tmp_writefds);
                                FD_CLR(fileD, &tmp_readfds);
                                fds_servers.erase(fileD);
                                close(fileD);
                            }
                        }
                    }
                }
            }
        }
    }

    void socketStart(std::vector<server> &servers)
    {

        Socket sockets; // initialize of sockets (multiple sockets) by how many server we have

        for (size_t i = 0; i < servers.size(); i++) // loop for create procces of each server
        {
            sockets.setPort(atoi(servers[i].get_port().c_str())); // setting the port of the server
            sockets.start(servers[i]);                            // starting the socket
            servers[i].setSocket(sockets.getSock());
            servers[i].setcheck(false);
        }
        connection_loop(servers);
    }
}