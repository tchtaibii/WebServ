/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchtaibi <tchtaibi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 15:02:16 by tchtaibi          #+#    #+#             */
/*   Updated: 2023/03/03 00:53:53 by tchtaibi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "parsingRequest.hpp"
#include "../confParsing/parsingConf.hpp"

namespace ws{
    class Socket
    {
        private:
            int     sock;
            int     connection;
            int     listening;
            int     backlog;
            int     new_socket;
            int     domain;
            u_long  interface;
            int     port;
            int     service;
            int     protocol;
            struct sockaddr_in address;
        public:
            Socket(int back_log = 10, int port = 8000, u_long interface = INADDR_ANY, int domain = AF_INET, int service = SOCK_STREAM, int protocol = 0)
            {
                this->backlog = back_log;
                this->port = port;
                this->interface = interface;
                this->domain = domain;
                this->service = service;
                this->protocol = protocol;
            }
            void start()
            {
                // Etablish socket
                sock = socket(domain,service, protocol);
                if (sock == 0)
                {
                    // perror("In socket...");
                    // exit(EXIT_FAILURE);
                }
                // adress struct
                address.sin_family = domain;
                address.sin_addr.s_addr = htonl(interface);
                address.sin_port = htons(port);
                memset(address.sin_zero, '\0', sizeof address.sin_zero);
                connection = bind(sock, (struct sockaddr *)&address, sizeof(address));
                test_connection(connection);
                listening = listen(sock, backlog);
                test_connection(listening);
                ft_accept();
            }
            void ft_accept()
            {
                int valread;
                std::string str;
                int addrlen = sizeof(address);
                while(1)
                {
                    // printf("\n+++++++ Waiting for new connection ++++++++\n\n");
                    new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    test_connection(new_socket);
                    char buffer[30000] = {0};
                    valread = read( new_socket , buffer, 30000);
                    // printf("%s\n",buffer );
                    write(new_socket , str.c_str() , str.length());
                    printf("------------------message sent-------------------\n");
                    HttpRequest req = parse_http_request(buffer);
                    // std::cout << "hello" << std::endl;
                    std::cout << "Method: " << req.method << std::endl;
                    std::cout << "Path: " << req.path << std::endl;
                    std::cout << "Version: " << req.version << std::endl;
                    std::cout << "Headers: " << std::endl;
                    for (std::map<std::string, std::string>::const_iterator it = req.headers.begin(); it != req.headers.end(); ++it)
                        std::cout << it->first << ": " << it->second << std::endl;
                    close(new_socket);
                }
            }
            void test_connection(int itemToTest)
            {
                if (itemToTest < 0)
                {
                    // perror("Connection Failed...");
                    exit(EXIT_FAILURE);
                }
            }
            // Getter
            struct sockaddr_in getAddress() {return address;}
            int getSock() {return sock;}
            int getConnection() {return connection;}
            int getListening() {return listening;}
            int getBacklog() {return backlog;}
            // Setter
            void setPort(int p) {this->port = p;}
            ~Socket(){}
    };
    void socketStart(std::vector<server> servers)
    {
        std::cout << "Start..." << std::endl;
    
        Socket sockets[servers.size()];

        for (int i = 0; i < servers.size(); i++) {
            pid_t pid = fork();
            if (pid == -1) {
                // error handling
            } else if (pid == 0) {
                sockets[i].setPort(atoi(servers[i].get_port().c_str()));  // child process
                sockets[i].start();
                exit(0);
            }
        }
        // wait for child processes to exit
        for (int i = 0; i < 3; i++) {
            wait(NULL);
        }
    }
}

