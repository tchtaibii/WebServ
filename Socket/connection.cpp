#include "socket.hpp"
namespace ws
{
    std::map<int, bool> ft_fds(std::vector<server> &servers)
    {
        std::map<int, bool> fds;
        for (size_t i = 0; i < servers.size(); i++)
            fds.insert(std::make_pair(servers[i].getSocket(), false));
        return fds;
        
    }
    void connection_loop(std::vector<server> servers)
    {
        std::map<int, bool> fds = ft_fds(servers);
        fd_set readfds;
        int valread;
        // int statusCode;
        FD_ZERO(&readfds);
        for (size_t i = 0; i < servers.size(); i++)
            FD_SET(servers[i].getSocket(), &readfds);
        fd_set tmp_readfds;
        while (1)
        {
            tmp_readfds = readfds;
            int max_fd = servers[0].getSocket();
            for (size_t i = 0; i < servers.size(); i++)
            {
                if (servers[i].getSocket() > max_fd)
                    max_fd = servers[i].getSocket();
            }
            int activity = select(max_fd + 1, &tmp_readfds, NULL, NULL, NULL);
            if ((activity < 0) && (errno != EINTR))
                printf("select error");
            int new_socket;
            for (int i = 0; i <= max_fd; i++)
            {
                // if (FD_ISSET(i, &readfds))
                // {
                // int addrlen = sizeof(servers[0].getAddress());
                // struct sockaddr_in address_tmp = servers[0].getAddress();
                new_socket = accept(i, NULL, NULL); // new FD that take the request
                FD_SET(new_socket, &readfds);
                if (new_socket > max_fd)
                    max_fd = new_socket;
                // servers[0].test_connection(new_socket);
                char buffer[30000] = {0};
                valread = read(new_socket, buffer, 30000); // reading the request from FD(new_socket) and stock it on buffer
                write(new_socket, NULL, 1);
                printf("------------------message sent-------------------\n");
                std::cout << buffer << std::endl;
                // HttpRequest req = parse_http_request(buffer);                                                                           // parsing the request
                // std::map<std::string, location>::iterator itLocation = servers[i].locationChecker(req.path, servers[i].get_location()); // getting the location from the server
                // if (itLocation != servers[i].get_location().end())                                                                      // check if the location is existing or not
                // {
                //     if (servers[i].methodChecker(req.method, itLocation->second.get_method())) // check if the method of request is existing on the location
                //     {
                //         // response
                //     }
                //     else
                //     {
                //         statusCode = 405;
                //     }
                // }
                // else // If a location is not found on the server
                // {
                //     statusCode = 404;
                // }
                // // buffer = NULL;
                // }
            }
            FD_CLR(new_socket, &readfds);
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
        }
        connection_loop(servers);
    }
}
