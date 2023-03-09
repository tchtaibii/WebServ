#pragma once
#include "socket.hpp"
#include <fcntl.h>
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
        std::vector<int> clients;
        fd_set readfds;
        // fd_set writefds;
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
            // Create a temporary fd_set to store the set of file descriptors
            // that are ready to be read from.
            fd_set tmp_readfds = readfds;

            // Wait for any of the file descriptors in the set to become
            // readable using the select
            if (select(max + 1, &tmp_readfds, NULL, NULL, NULL) < 0)
                throw std::runtime_error("Select error");
            // Loop through all possible file descriptors, checking if each
            // one is in the set of readable file descriptors.
            for (int fileD = 0; fileD <= max; fileD++)
            {
                if (FD_ISSET(fileD, &tmp_readfds))
                {
                    // If the file descriptor is a listening socket, accept a new
                    // connection and add the new socket to the set of readable
                    // file descriptors.
                    if (std::count(fds.begin(), fds.end(), fileD))
                    {
                        new_socket = accept(fileD, NULL, NULL);
                        fcntl(new_socket, F_SETFL, O_NONBLOCK);
                        FD_SET(new_socket, &readfds);
                        clients.push_back(new_socket);
                        if (new_socket > max)
                            max = new_socket;
                    }
                    // If the file descriptor is a client socket, read data from it.
                    else
                    {
                        char buffer[30001] = {0};
                        int valread = recv(fileD, buffer, 30000, 0);
                        if (valread <= 0)
                        {
                            // If the client socket has been closed or disconnected,
                            // remove it from the set of readable file descriptors and
                            // close the socket.
                            clients.erase(std::remove(clients.begin(), clients.end(), fileD), clients.end());
                            fds.erase(std::remove(fds.begin(), fds.end(), fileD), fds.end());
                            close(fileD);
                            FD_CLR(fileD, &readfds);
                            max = *std::max_element(clients.begin(), clients.end());
                        }
                        else if (valread > 0)
                        {
                            // If data has been read from the client socket, process
                            // the request and send a response back to the client.
                            std::cout << buffer << std::endl;
                            HttpRequest req = parse_http_request(buffer);
                            send(fileD, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 14\r\n\r\nHello, World!\r\n", 100, 0);
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