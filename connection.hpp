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
        int max = 0;
        int new_socket;
        FD_ZERO(&readfds);
        for (std::map<int, server>::iterator it = fds_servers.begin(); it != fds_servers.end(); it++)
        {
            fds.push_back(it->first);
            FD_SET(it->first, &readfds);
            if (it->first > max)
                max = it->first;
        }
        while (1)
        {
            fd_set tmp_readfds = readfds;
            if (select(max + 1, &tmp_readfds, NULL, NULL, NULL) < 0)
                throw std::runtime_error("Select error");
            for (int fileD = 0; fileD <= max; fileD++)
            {
                if (FD_ISSET(fileD, &tmp_readfds))
                {
                    if (std::count(fds.begin(), fds.end(), fileD))
                    {
                        new_socket = accept(fileD, NULL, NULL);
                        fcntl(new_socket, F_SETFL, O_NONBLOCK);
                        FD_SET(new_socket, &readfds);
                        clients.push_back(new_socket);
                        if (new_socket > max)
                            max = new_socket;
                    }
                    else
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
                        }
                        else if (valread > 0)
                        {
                            std::string request_str = std::string(buffer, valread);
                            if (!req.deja)
                            {
                                req = parse_http_request(request_str, req);
                                tmp_body = req.body;
                                req.body = "";
                                std::cout << "Method: " << req.method << std::endl;
                                std::cout << "Path: " << req.path << std::endl;
                                std::cout << "Version: " << req.version << std::endl;
                                std::cout << "Headers:" << std::endl;
                                for (std::map<std::string, std::string>::iterator it = req.headers.begin(); it != req.headers.end(); it++)
                                {
                                    std::cout << "  " << it->first << ": " << it->second << std::endl;
                                }
                                std::cout << "body : ";
                                // std::cout << tmp_body;
                            }
                            else
                            {
                                bool end_with_0 = false;
                                if (!req.con && req.method == "POST")
                                {
                                    // std::cout << request_str;
                                    if (req.chunked == true)
                                    {
                                        // std::cout << "yes is chunked" << std::endl;
                                        end_with_0 = isZero(request_str);
                                    }
                                    tmp_body += request_str;
                                    req.con = bodyParsing(req, tmp_body, end_with_0);
                                }
                                else{
                                    
                                }
                            }
                        }
                        else if (valread == 0)
                        {
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
                            throw std::runtime_error("Read error");
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