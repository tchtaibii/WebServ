#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "../Socket/socket.hpp"
// #include "../methods/getMethod.hpp"
#include "../Response/Response.hpp"
#include <unistd.h>
#include <fcntl.h>

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
		std::map<std::string, std::string> cgi;
		std::map<std::string, location> _location;
		bool checker_flag;
		int status;
		std::string body;
		std::string path;
		response _response;
		int i;
		int fd;
		bool dir;

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

		bool methodChecker(std::string method, std::vector<std::string> Location)
		{
			for (size_t n = 0; n < Location.size(); n++)
				if (Location[n] == method)
					return true;
			return false;
		}

		void getMethod(std::string Location)
		{
			std::string file = this->get_location()[Location].get_root() + this->get_location()[Location].get_default();
			if (Location != req.path)
				file = pathjoin(this->get_location()[Location].get_root(), req.path);
			this->path = file;
			if (fileExists(file))
			{
				if (is_directory(file))
				{
					if (req.path.back() != '/')
						status = 301;
					else if (access(file.c_str(), R_OK) || this->_location[req.path].get_autoindex() == "off")
						status = 402;
					else
						status = 200;
					dir = true;
					return ;
				}
				status = 200;
				return;
			}
			status = 404;
		}

	public:
		server(){
			status = 0;
			i = 0;
		}
	
		ws::HttpRequest req;
		int flg;
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
		std::map<std::string, std::string> &get_cgi() { return this->cgi; }
		void set_cgi(const std::map<std::string, std::string> &c) { this->cgi = c; }
		std::map<std::string, ws::location> &get_location() { return this->_location; }
		void set_location(const std::map<std::string, ws::location> &a) { this->_location = a; }
		void set_req(ws::HttpRequest reqi)
		{
			reqi.body.clear();
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
			std::map<std::string, std::string> hed(req.headers.begin(), req.headers.end());
			std::string a = hed["Transfer-Encoding"];
			std::string C = hed["Content-Length"];
			// std::string R = _location[req.path].get_redirect().find("301")->second;
			// if (!R.empty())
			// 	status = 301;
			// std::cout << _location[req.path].get_redirect().find("301")->second << std::endl;
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
				for (int i = 0; req.path[i]; i++)
				{
					if ((isalnum(req.path[i])) || (req.path[i] == 33) || (req.path[i] >= 35 && req.path[i] <= 47) || (req.path[i] <= 60 && req.path[i] >= 57) || (req.path[i] == 61) || (req.path[i] >= 63 && req.path[i] <= 64) || (req.path[i] == 95) || (req.path[i] == 126))
						i++;
					else
						status = 400;
				}
			}
		}

		void checker()
		{
			std::map<std::string, location> l = this->get_location();
			std::map<std::string, location>::iterator it = locationChecker(req.path, l);
			// if (!methodChecker(req.method, l[it->first].get_method()))
			// 	status = 405;
			// if (status == 405)
			// 	exit(1);
			if (req.method == "GET" && !status)
				getMethod(it->first);
			// else if (req.method == "POST" && !status)
			//     status =  PostMethod();
			// else if (req.method == "DELETE" && !status)
			//     status = DeleteMethod();
			// _response = _response();
			// std::cout << req.path << std::endl;
			// return responseFunction(req.path, req, status);
		}

		void response()
		{
			if (!_response.first_time)
			{
				if (status == 301 && !dir)
					this->_response.set_header(_location[req.path].get_redirect().find("301")->second, status, req, dir);
				else if (status == 301 && dir)
					this->_response.set_header(req.path + '/', status, req, dir);
				else
					this->_response.set_header(this->path, status, req, dir);
				send(this->socket, _response.response_header.c_str(), _response.response_header.length(), 0);
				if (!dir && status != 301)
					fd = open(_response.file_path.c_str(), O_RDONLY);
				this->_response.done = false;
			}
			if (dir || status == 301)
			{
				if (status != 301)
					send(this->socket, _response.dir_body.c_str(), _response.dir_body.length(), 0);
				this->_response.done = true;
				this->_response.first_time = false;
				status = 0;
				return ;
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
			try 
			{
				i += send(this->socket, buffer, sizeof(buffer), 0);
			}
			catch (...)
			{
				std::cerr << "a\n";
			}
		}
	};
}