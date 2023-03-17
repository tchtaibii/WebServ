#pragma once
#include "Socket/socket.hpp"
#include <fcntl.h>
#include "requestParsing/checkRequest.hpp"
#define READ_N 65536
namespace ws
{
    void change_socket(std::map<int, server> &fds_server, int fileD, int newSocket)
    {
        server tmp = fds_server[fileD];
        fds_server.erase(fileD);
        fds_server.insert(std::make_pair(newSocket, tmp));
    }
    std::map<int, server> ft_fds(std::vector<server> &servers)
    {
        std::map<int, server> fds;
        for (size_t i = 0; i < servers.size(); i++)
        {
            std::cout << servers[i].getSocket() << std::endl;
            fds.insert(std::make_pair(servers[i].getSocket(), servers[i]));
        }
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
                                std::cout << request_str;
                                if (!req.deja)
                                {
                                    req = parse_http_request(request_str, req, request_im);
                                    // req.body.clear();
                                    if (!req.headers_complet)
                                        continue;
                                    request_im.clear();
                                    tmp_body = req.body;
                                }
                                else
                                {
                                    if (!req.con && req.method == "POST")
                                    {
                                        if (req.chunked)
                                        {
                                            // req.end_ = isZero(request_str);
                                            remove_chunk_coding(request_str, req);
                                            request_str.clear();
                                        }
                                        else
                                        {
                                            tmp_body += request_str;
                                            request_str.clear();
                                            req.con = bodyParsing(req, tmp_body, req.end_);
                                        }
                                        if (req.con)
                                        {
                                            FD_SET(fileD, &writefds);
                                            FD_CLR(fileD, &readfds);
                                            req.con = 0;
                                            continue;
                                        }
                                    }
                                    else
                                    {
                                        request_im.clear();
                                        FD_SET(fileD, &writefds);
                                        FD_CLR(fileD, &readfds);
                                        continue;
                                    }
                                }
                                // if the body is so small
                                if ((req.method == "POST" && !req.headers["Content-Length"].empty() && (size_t)atoi(req.headers["Content-Length"].c_str()) == tmp_body.length()))
                                {
                                    if (req.chunked)
                                        remove_chunk_coding(request_str, req);
                                    req.con = bodyParsing(req, tmp_body, 1);
                                    FD_SET(fileD, &writefds);
                                    FD_CLR(fileD, &readfds);
                                    httpRequestInit(req, 0);
                                    request_im.clear();
                                    tmp_body.clear();
                                    continue;
                                }
                            }
                            else if (valread == 0)
                            {
                                httpRequestInit(req, 0);
                                clients.erase(std::remove(clients.begin(), clients.end(), fileD), clients.end());
                                fds.erase(std::remove(fds.begin(), fds.end(), fileD), fds.end());
                                close(fileD);
                                FD_CLR(fileD, &readfds);
                                max = *std::max_element(clients.begin(), clients.end());
                                continue;
                                // Clear the req.body buffer for the next request
                                // int errorFlag = is_req_well_formed(req);
                                // if (!errorFlag)
                                // {
                                // If the request is well-formed, process it
                                // ...
                                // After processing the request, send a response back to the client
                                // std::string response_str = generate_http_response(req);
                                // send(fileD, response_str.c_str(), response_str.length(), 0);
                                // }
                                // else
                                // {
                                //     // If the request is not well-formed, send a 400 Bad Request response back to the client
                                // std::string response_str = "HTTP/1.1 400 Bad Request\r\n\r\n";
                                // send(fileD, response_str.c_str(), response_str.length(), 0);
                                // }
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
                            request_im.clear();
                            tmp_body.clear();
                            // std::cout << "hhshshshshhs*****" << std::endl;
                            std::cout << fileD << std::endl;
                            server tmp_server = fds_servers[fileD];
                            std::cout << tmp_server.get_port() << std::endl;
                            httpRequestInit(req, 1);
                            FD_CLR(fileD, &writefds);
                            FD_CLR(fileD, &readfds);
                            FD_CLR(fileD, &tmp_writefds);
                            FD_CLR(fileD, &tmp_readfds);
                        }
                    }
                }
            }
        }
    }

    void socketStart(std::vector<server> &servers)
    {
        std::cout << "Start..." << std::endl;

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