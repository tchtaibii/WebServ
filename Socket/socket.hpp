/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchtaibi <tchtaibi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 15:02:16 by tchtaibi          #+#    #+#             */
/*   Updated: 2023/03/01 17:24:41 by tchtaibi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace ws{
    class Socket
    {
        private:
            int sock;
            int connection;
            int listening;
            int backlog;
            int new_socket;
            int domain;
            u_long interface;
            int port;
            int service;
            int protocol;
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
                std::string hello = "Hello from server";
                int addrlen = sizeof(address);
                while(1)
                {
                    // printf("\n+++++++ Waiting for new connection ++++++++\n\n");
                    new_socket = accept(sock, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                    test_connection(new_socket);
                    char buffer[30000] = {0};
                    valread = read( new_socket , buffer, 30000);
                    printf("%s\n",buffer );
                    write(new_socket , hello.c_str() , hello.length());
                    printf("------------------Hello message sent-------------------\n");
                    close(new_socket);
                }
            }
            void test_connection(int itemToTest)
            {
                if (itemToTest < 0)
                {
                    // perror("Connection Failed...");
                    // exit(EXIT_FAILURE);
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
}

