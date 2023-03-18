/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchtaibi <tchtaibi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 15:02:16 by tchtaibi          #+#    #+#             */
/*   Updated: 2023/03/09 18:24:16 by tchtaibi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "../Request/Request.hpp"
#include "../Config/parsingConf.hpp"

namespace ws
{
    class Socket
    {
    private:
        int sock;
        int connection;
        int listening;
        int backlog;
        // int new_socket;
        int domain;
        u_long interface;
        int port;
        int service;
        int protocol;
        struct sockaddr_in address;

    public:
        Socket(int back_log = 10000, int port = 8000, u_long interface = INADDR_ANY, int domain = AF_INET, int service = SOCK_STREAM, int protocol = 0)
        {
            this->backlog = back_log;
            this->port = port;
            this->interface = interface;
            this->domain = domain;
            this->service = service;
            this->protocol = protocol;
        }
        void start(server servers)
        {
            // Etablish socket
            (void)servers;
            sock = socket(domain, service, protocol);
            if (sock == 0)
            {
                perror("In socket...");
                exit(EXIT_FAILURE);
            }
            int on = 1024;
            setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
            // init adress struct
            address.sin_family = domain;
            address.sin_addr.s_addr = htonl(interface);
            address.sin_port = htons(port);
            memset(address.sin_zero, '\0', sizeof address.sin_zero);
            // binding the connection
            connection = bind(sock, (struct sockaddr *)&address, sizeof(address));
            test_connection(connection);
            // listen
            listening = listen(sock, backlog);
            test_connection(listening);
            // ft_accept(servers);
        }
        std::map<std::string, location>::iterator locationChecker(std::string path, std::map<std::string, location> &Location)
        {
            if (path == "/")
                return Location.find(path);
            std::vector<std::string> pathComponents; // vector of paths
            std::stringstream ss(path);
            std::string component;            // tmp of one path
            std::getline(ss, component, '/'); // getting the first path '/'
            std::string tmp = "\0";
            pathComponents.push_back("/" + component);
            while (std::getline(ss, component, '/')) // loop for getting paths and stock it on the vector(pathComponents)
            {
                pathComponents.push_back(tmp + "/" + component);
                tmp += "/" + component;
            }
            std::map<std::string, location>::iterator it;
            for (size_t n = pathComponents.size(); n > 0; n--) // check if one of the pathComponents is exiting on the server(location)
            {
                it = Location.find(pathComponents[n - 1]);
                std::cout << "skchhh" << std::endl;
                if (it != Location.end())
                    return it;
            }
            return Location.end(); // if isn't exist we return end of map
        }
        bool methodChecker(std::string method, std::vector<std::string> Location)
        {
            for (size_t n = 0; n < Location.size(); n++)
                if (Location[n] == method)
                    return true;
            return false;
        }
        // void ft_accept(server servers)
        // {
        //     int valread;
        //     int addrlen = sizeof(address);
        //     int statusCode;
        //     while (1)
        //     {
        //         new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t *)&addrlen); // new FD that take the request
        //         test_connection(new_socket);
        //         char buffer[1024] = {0};
        //         valread = read(new_socket, buffer, 30000); // reading the request from FD(new_socket) and stock it on buffer
        //         write(new_socket, NULL, 1);
        //         printf("------------------message sent-------------------\n");
        //         std::cout << buffer << std::endl;
        //         HttpRequest req = parse_http_request(buffer);                                                             // parsing the request
        //         std::map<std::string, location>::iterator itLocation = locationChecker(req.path, servers.get_location()); // getting the location from the server
        //         if (itLocation != servers.get_location().end())                                                           // check if the location is existing or not
        //         {
        //             if (methodChecker(req.method, itLocation->second.get_method())) // check if the method of request is existing on the location
        //             {
        //                 // response
        //             }
        //             else
        //             {
        //                 // error page
        //             }
        //         }
        //     }
        // }
        void test_connection(int itemToTest)
        {
            if (itemToTest < 0)
            {
                perror("Connection Failed...");
                exit(EXIT_FAILURE);
            }
        }
        // Getter
        struct sockaddr_in getAddress() { return address; }
        int getSock() { return sock; }
        int getPort() { return port; }
        int getConnection() { return connection; }
        int getListening() { return listening; }
        int getBacklog() { return backlog; }
        // Setter
        void setPort(int p) { this->port = p; }
        ~Socket() {}
    };
}
