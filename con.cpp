void connection_loop(std::vector<server> &servers)
{
    std::map<int, server> fds = ft_fds(servers);
    fd_set readfds;
    std::vector<int> clients;
    // int statusCode;
    int max_fd = 0;
    FD_ZERO(&readfds);
    for (std::map<int, server>::iterator it = fds.begin(); it != fds.end(); it++)
    {
        FD_SET(it->first, &readfds);
        if (it->first > max_fd)
            max_fd = it->first;
    }
    fd_set tmp_readfds;
    while (1)
    {
        bool v = false;
        tmp_readfds = readfds;
        // for (size_t i = 0; i < servers.size(); i++)
        // {
        //     if (servers[i].getSocket() > max_fd)
        //         max_fd = servers[i].getSocket();
        // }
        if (select(max_fd + 1, &tmp_readfds, NULL, NULL, NULL) < 0)
            throw std::runtime_error("Select error");
        int new_socket;
        for (size_t i = 0; i < servers.size(); i++)
        {
            if (FD_ISSET(servers[i].getSocket(), &tmp_readfds))
            {
                // int addrlen = sizeof(servers[0].getAddress());
                // struct sockaddr_in address_tmp = servers[0].getAddress();
                // if (fds[i].getcheck() == false)
                new_socket = accept(servers[i].getSocket(), NULL, NULL); // new FD that take the request
                fcntl(new_socket, F_SETFL, O_NONBLOCK);
                FD_SET(new_socket, &readfds);
                clients.push_back(new_socket);
                // fds[i].setcheck(true);
                if (new_socket > max_fd)
                    max_fd = new_socket;
                v = true;
                // servers[0].test_connection(new_socket);

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
            }
        }
        if (v == true)
            continue;
        // std::cout << "-----> "  << clients.size() << std::endl;
        for (size_t i = 0; i < clients.size(); i++)
        {
            if (FD_ISSET(clients[i], &tmp_readfds))
            {
                char buffer[30001] = {0};
                // usleep(500);
                int valread = recv(clients[i], buffer, 30000, 0);
                buffer[valread] = 0; // reading the request from FD(new_socket) and stock it on buffer
                printf("------------------message sent------------------- lenght : %d\n", valread);
                std::cout << buffer << std::endl;
                sleep(2);
                send(clients[i], "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 14\r\n\r\nHello, World!\r\n", 100, 0);
                sleep(5);
                // exit (0);
            }
        }
        // FD_CLR(new_socket, &readfds);
    }
}