#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "../Socket/socket.hpp"
#include "../Response/Response.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>

namespace ws
{
	class location
	{
	private:
		std::vector<std::string> method;
		std::string root;
		std::string autoindex;
		std::string _default;
		std::string upload;
		std::map<std::string, std::string> redirect;

	public:
		bool		cgi;
		int flg;
		std::vector<std::string> &get_method() { return this->method; }
		void set_method(const std::vector<std::string> &b) { this->method = b; }
		std::string const &get_root() const { return this->root; }
		void set_root(const std::string &b) { this->root = b; }
		std::string const &get_autoindex() const { return this->autoindex; }
		void set_autoindex(const std::string &b) { this->autoindex = b; }
		std::string const &get_default() const { return this->_default; }
		void set_default(const std::string &b) { this->_default = b; }
		std::string const &get_upload() const { return this->upload; }
		void set_upload(const std::string &b) { this->upload = b; }
		std::map<std::string, std::string> &get_redirect() { return this->redirect; }
		void set_redirect(const std::map<std::string, std::string> &b) { this->redirect = b; }
	};

	class server
	{
	private:
		std::string port;
		int socket;
		std::string host;
		std::string server_name;
		std::string body_size;
		std::string error_page;
		std::map<std::string, location> _location;
		bool checker_flag;
		int status;
		std::string body;
		std::string path;
		response _response;
		int i;
		int fd;
		bool dir;
		std::string Location;

		bool methodChecker(std::string method, std::vector<std::string> Location)
		{
			for (size_t n = 0; n < Location.size(); n++)
				if (Location[n] == method)
					return true;
			return false;
		}

		void PostMethod(std::string Location)
		{
			this->path = this->get_location()[Location].get_root() + this->get_location()[Location].get_default();
			if (Location != req.path)
				this->path = pathjoin(this->get_location()[Location].get_root(), req.path, this->Location);
			std::cout << path << std::endl;
			if (fileExists(path))
			{
				std::cout << "EXist\n";
				if (is_directory(path))
				{
					if (req.path.back() != '/')
					{
						status = 301;
						dir = true;
					}
					else if (access(path.c_str(), R_OK)
						|| !check_file(path).empty())
					{
						path = check_file(path, 0);
						status = 200;
					}
					else
						status = 403;
					return ;
				}
				if (!check_extension2(path))
					status = 403;
				else
					status = 200;
				return ;
			}
			status = 404;
		}

		void getMethod(std::string Location)
		{
			path = this->get_location()[Location].get_root() + this->get_location()[Location].get_default();
			if (Location != req.path)
				path = pathjoin(this->get_location()[Location].get_root(), req.path, this->Location);
			if (fileExists(path))
			{
				if (is_directory(path))
				{
					if (req.path.back() != '/')
						status = 301;
					else if (!check_file(path, 0).empty())
					{
						path = check_file(path, 0);
						status = 200;
						return ;
					}	
					else if (access(path.c_str(), R_OK)
						|| this->_location[Location].get_autoindex() == "off")
						status = 403;
					else
						status = 200;
					dir = true;
					return;
				}
				status = 200;
				return;
			}
			status = 404;
		}

		void DeleteMethod(const std::string &Location)
		{
			std::string file = this->get_location()[Location].get_root() + this->get_location()[Location].get_default();
			if (Location != req.path)
				file = pathjoin(this->get_location()[Location].get_root(), req.path, Location);
			this->path = file;
			if (fileExists(file))
			{
				if (is_directory(file))
				{
					if (req.path.back() != '/')
						status = 409;
					else if (access(file.c_str(), W_OK))
						status = 403;
					else if (remove_directory(path))
						status = 204;
					return;
				}
				if (!std::remove(path.c_str()))
					status = 204;
				return;
			}
			status = 404;
		}

	public:
		server()
		{
			status = 0;
			i = 0;
			this->_response.first_time = false;
		}

		std::map<std::string, location>::iterator locationChecker(std::string path, std::map<std::string, location> &Location)
		{
			std::vector<std::string> pathComponents; // vector of paths
			std::stringstream ss(path);
			std::string component;			  // tmp of one path
			std::getline(ss, component, '/'); // getting the first path '/'
			std::string tmp = "\0";
			pathComponents.push_back("/" + component);
			while (std::getline(ss, component, '/')) // loop for getting paths and stock it on the vector(pathComponents)
			{
				pathComponents.push_back(tmp + "/" + component);
				tmp += "/" + component;
			}
			if (pathComponents.size() == 1)
				return Location.find("/");
			std::map<std::string, location>::iterator it;
			size_t n = pathComponents.size() - 1;
			for (; n > 0; n--) // check if one of the pathComponents is exiting on the server(location)
			{
				it = Location.find(pathComponents[n]);
				if (it != Location.end())
					return it;
			}
			return Location.find("/"); // if isn't exist we return end of map
		}

		ws::HttpRequest req;
		int flg;
		void setStatus(int st){ this->status = st;}
		std::string const &get_port() const { return this->port; }
		int const &getSocket() const { return this->socket; }
		void setSocket(const int &sock) { this->socket = sock; }
		bool const &getcheck() const { return this->checker_flag; }
		void setcheck(const bool &b) { this->checker_flag = b; }
		void set_port(const std::string &p) { this->port = p; }
		std::string const &get_host() const { return this->host; }
		void set_host(const std::string &host) { this->host = host; }
		std::string const &get_server_name() const { return this->server_name; }
		void set_server_name(const std::string &server_name) { this->server_name = server_name; }
		std::string const &get_body_size() const { return this->body_size; }
		void set_body_size(const std::string &a) { this->body_size = a; }
		std::string const &get_error_page() const { return this->error_page; }
		void set_error_page(const std::string &e) { this->error_page = e; }
		// std::map<std::string, std::string> &get_cgi() { return this->cgi; }
		// void set_cgi(const std::map<std::string, std::string> &c) { this->cgi = c; }
		std::map<std::string, ws::location> &get_location() { return this->_location; }
		void set_location(const std::map<std::string, ws::location> &a) { this->_location = a; }
		void set_req(ws::HttpRequest reqi)
		{
			// reqi.body.clear();
			this->req = reqi;
		}
		ws::HttpRequest get_req() { return this->req; }
		std::string get_body() { return this->body; }
		void set_body(std::string &b) { this->body = b; }
		int get_status() { return this->status; };
		// void										set_status(int status) { this->status = status; }
		bool getDone() { return this->_response.done; }
		void is_req_well_formed()
		{
			this->Location = locationChecker(req.path, this->get_location())->first;
			std::cout << "Location = " << Location << std::endl;
			std::map<std::string, std::string> hed(req.headers.begin(), req.headers.end());
			std::string a = hed["Transfer-Encoding"];
			std::string C = hed["Content-Length"];
			std::string R;
			if (_location[this->Location].get_redirect().find("301") != _location[this->Location].get_redirect().end())
				R = _location[this->Location].get_redirect().find("301")->second;
			if (!R.empty())
			{
				path = R;
				status = 301;
			}
			if (!a.empty() && a != "chunked\r")
				status = 501;
			else if (a.empty() && C.empty() && req.method == "POST")
				status = 400;
			else if (req.path.length() > 2048)
				status = 414;
			else if (a.empty() && C.empty() && req.method == "POST")
				status = 400;
			else if (!C.empty() && atoi(C.c_str()) > atoi(this->body_size.c_str()))
				status = 413;
			else
			{
				for (size_t i = 0; i < req.path.length(); i++)
				{
					if ((isalnum(req.path[i])) || (req.path[i] == 33)
						|| (req.path[i] >= 35 && req.path[i] <= 47)
							|| (req.path[i] <= 60 && req.path[i] >= 57) || (req.path[i] == 61)
								|| (req.path[i] >= 63 && req.path[i] <= 64) || (req.path[i] == 95)
									|| (req.path[i] == 126))
						i++;
					else
					{
						status = 400;
						break ;
					}
				}
			}
			std::cout << "status = " << status << std::endl;
		}

		void checker()
		{
			std::cout << "checker " << std::endl;
			std::map<std::string, location> l = this->get_location();
			if (!methodChecker(req.method, l[Location].get_method()))
				status = 405;
			std::cout << "hey\n";
			std::cout << req.NoUpload << std::endl;
			if (!req.NoUpload && req.method == "POST")
			{
				status = 201;
				path = this->_location[Location].get_root() + this->_location[Location].get_upload().substr(1);
				std::cout << "=-=-=-" << path << std::endl;
			}
			else if (req.method == "GET" && !status)
				getMethod(Location);
			else if (req.method == "DELETE" && !status)
				DeleteMethod(Location);
			else if (req.method == "POST" && !status)
			    PostMethod(Location);
		}

		void response()
		{
			std::cout << "lol\n";
			if (!_response.first_time)
			{
				req.port = this->port;
				std::cout << "here " << status << std::endl;
				if (status == 301 && dir)
					this->_response.set_header(req.path + '/', status, req, dir, this->error_page, this->_location[Location].cgi);
				else
					this->_response.set_header(this->path, status, req, dir, this->error_page, this->_location[Location].cgi);
				if ((!dir && status != 301) || (dir && status == 403))
					fd = open(_response.file_path.c_str(), O_RDONLY);
				_response._send(_response.response_header.c_str(), this->socket, _response.response_header.length());
				this->_response.done = false;
				return ;
			}
			if ((dir && status != 403) || status == 301 || _response.errors)
			{
				std::cout << status << std::endl;
				if (status != 301)
					_response._send(_response.dir_body.c_str(), this->socket, _response.dir_body.length());
				this->_response.done = true;
				this->_response.first_time = false;
				status = 0;
				return;
			}
			char buffer[1024];
			lseek(fd, i, SEEK_SET);
			if (read(fd, buffer, sizeof(buffer)) <= 0)
			{
				this->_response.done = true;
				this->_response.first_time = false;
				i = 0;
				status = 0;
				close(fd);
			}
			int x = 0; 
			x += _response._send(buffer, this->socket, sizeof(buffer));
			if (x == -1)
			{
				this->_response.done = true;
				this->_response.first_time = false;
				i = 0;
				status = 0;
				close(fd);
			}
			else
				i += x;
			if (_response._cgi)
				remove(_response.file_path.c_str());
		}
	};
}