void connection::network_core(parsefile s)
{
	std::map<int, request> chunking_map;
	std::map<int, request> serving_map;
	std::map<int, Response*> response_map;
	std::map<int, std::vector<server_config> > fdServer_map;
	std::map<int, std::vector<server_config> > fdClient_map;
	std::vector<int> listOffd;
	int activity;
	int new_socket;
	server_config serv;
	fd_set writefds;
	fd_set readfds;
	int max = 0;
	int flag = 0;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
    for (std::map<int, int>::iterator it = s.get_ports().begin(); it != s.get_ports().end(); ++it)
	{
        it->second = create_server(it->first);
		FD_SET(it->second, &readfds);
		listOffd.push_back(it->second);
		fdServer_map.insert(std::make_pair(it->second, get_servers_with_same_port(s, it->first)));
		std::cout << "port: " << it->first << " fd: " << it->second << std::endl;
		if (it->second > max)
			max = it->second;
	}
	fd_set copy_read = readfds;
	fd_set copy_write = writefds;
	s.set_ports(switch_map(s.get_ports()));
	signal(SIGPIPE, SIG_IGN);
	while (true)
	{
		readfds = copy_read;
		writefds = copy_write;
		activity = select(max + 1, &readfds, &writefds, NULL, NULL);
		if (activity < 0)
			throw std::runtime_error("Select error");
		for (int fd = 0; fd <= max; fd++)
		{
			if (FD_ISSET(fd, &readfds) || FD_ISSET(fd, &writefds))
			{
				if (std::count(listOffd.begin(), listOffd.end(), fd))
				{
					new_socket = accept(fd, NULL, NULL);
					fcntl(new_socket, F_SETFL, O_NONBLOCK);
					FD_SET(new_socket, &copy_read);
					fdClient_map.insert(std::make_pair(new_socket, get_servers_with_same_port(s, s.get_ports()[fd])));
					if (new_socket > max)
						max = new_socket;
					continue;
				}
				else
				{
					std::vector<server_config> server = fdClient_map[fd];
					if (FD_ISSET(fd, &readfds))
					{
						int ret = read(fd, buffer, SIZE_OF_BUFFER);
						if (ret == -1)
						{
							close(fd);
							FD_CLR(fd, &copy_read);
							fdClient_map.erase(fd);
							listOffd.erase(std::remove(listOffd.begin(), listOffd.end(), fd), listOffd.end());
						}
						else if (ret > 0)
						{
							if (chunking_map.find(fd) == chunking_map.end())
							{
								request	req(buffer, ret);
								if (req.get_is_complete())
								{
									FD_SET(fd, &copy_write);
									FD_CLR(fd, &copy_read);
									serving_map.insert(std::make_pair(fd, req));
								}
								else
									chunking_map.insert(std::make_pair(fd, req));
							}
							else if (chunking_map.find(fd) != chunking_map.end())
							{
								chunking_map[fd].fill_body(buffer, 2, ret);
								if (chunking_map[fd].get_is_complete())
								{
									FD_SET(fd, &copy_write);
									FD_CLR(fd, &copy_read);
									serving_map.insert(std::make_pair(fd, chunking_map[fd]));
									chunking_map.erase(fd);
								}
							}
						}
						else if (ret == 0)
						{
							close(fd);
							FD_CLR(fd, &copy_read);
							fdClient_map.erase(fd);
							listOffd.erase(std::remove(listOffd.begin(), listOffd.end(), fd), listOffd.end());
						}
						else
							throw std::runtime_error("Read error");
					}
					else
					{
						char *buffer_new;
						server_config tmp = get_server_by_host(server, serving_map[fd].get_host());
						if (response_map.find(fd) != response_map.end())
						{
							// std::cout << "first!" << std::endl;
							size_t size = 0;
							buffer_new = get_buffer(response_map[fd]->get_written(), response_map[fd]->get_content_lenght(), response_map[fd]->get_body(), &size);
							write(fd, buffer_new, size);
							// std::cout << "Sec time!" << std::endl;
							free(buffer_new);
							flag++;
							// std::cout << "Body: " << response_map[fd]->get_body() << std::endl;
							// std::cout << "Header: " << response_map[fd]->get_header() << std::endl;
							response_map[fd]->get_body();
							response_map[fd]->set_written(size + response_map[fd]->get_written());
							if (response_map[fd]->get_written() >= response_map[fd]->get_content_lenght())
							{
								FD_CLR(fd, &copy_write);
								// std::cout << "Done!" << std::endl;
								delete response_map[fd];
								serving_map.erase(fd);
								response_map.erase(fd);
								close(fd);
							}
							continue;
						}
						Response *rep = new Response(tmp, serving_map[fd]);
						if (!rep->get_is_file())
						{
							int w = write(fd, rep->get_header().c_str(), rep->get_header().size());
							// std::cout << "enterd!" << std::endl;
							//  std::cout << "Write: " << w << std::endl;
							// std::cout << "Size: " << rep->get_header().size() << std::endl;
							FD_CLR(fd, &copy_write);
							serving_map.erase(fd);
							delete rep;
							close(fd);
						}
						else if (response_map.find(fd) == response_map.end())
						{
							size_t size = 0;
							if (rep->get_is_complete())
							{
								// std::cout << "--------------Response-----------------" << std::endl;
								// std::cout << "Header: " << rep->get_header() << std::endl;
								// std::cout << "---------------------------------------" << std::endl;
								buffer_new = get_buffer_with_headers(rep, &size);
								write(fd, buffer_new, size);
								FD_CLR(fd, &copy_write);
								free(buffer_new);
								serving_map.erase(fd);
								delete rep;
								close(fd);
							}
							else
							{
								buffer_new = get_buffer_with_headers(rep, &size);
								response_map.insert(std::make_pair(fd, rep));
								rep->set_written(size - rep->get_header().size());
								write(fd, buffer_new, size);
								free(buffer_new);
							}
						}
					}
				}
			}
		}
	}
}