#pragma once
#include <dirent.h>
#include "../Request/tools.hpp"
#include "../cgi/cgi.hpp"
#include "../Request/tools.hpp"

class response 
{
    public :
        bool		done;
        bool		first_time;
		std::string response_header;
		std::string response_body;
		std::string file_path;
		bool		errors;
		std::string dir_body;
		std::string cgi_file_path;
		int			i;
		int			fd;
		bool		_cgi;
		std::string content_type;

        void    set_header(std::string file, int status, ws::HttpRequest req, bool dir, std::string &error_page, bool cgi_true)
        {
			this->first_time = true;
			this->file_path = file;
			_cgi = false;
			errors = false;
			if (dir && status != 301 && status != 403)
				setDirheader();
			if (status != 200 && status != 301 && status != 204 && !error_page.empty())
				get_path(status, error_page);
			else if ((error_page.empty() && status != 200 && status != 301 && status != 204)
				|| status == 201)
			{
				errors = true;
				set_error(status);
			}

			if (cgi_true && !errors && check_extension2(file_path) && req.method != "DELETE")
			{
				cgi c(file_path, req);
				c.exec();
				_cgi = true;
				file_path = c.get_outfile_path();
				content_type = c.get_content_type();
			}

			std::ostringstream oss;
			oss <<  req.version + response_message(status);
			oss << "Date: " << getCurrentDate() << "\r\n";
			if (status != 204)
			{
				if (status == 301 || status == 201)
				{
					oss << "Location: " << file << "\r\n";
					if (status == 301)
						oss << "Content-Length: " << '0' << "\r\n";
				}
				else
				{
					if (errors)
						oss << "Content-Type: " <<  "text/html" << "\r\n";
					else if (_cgi)
						oss << content_type << "\r\n";
					else
						oss << "Content-Type: " <<  check_MIME(file_path, dir) << "\r\n";
					if (status != 209)
					{
						if ((!dir || (dir && status == 403)) && !errors)
							oss << "Content-Length: " << get_size(file_path) << "\r\n";
						else
							oss << "Content-Length: " << dir_body.length() << "\r\n";
					}
				}
			}
			oss << "\r\n";
			this->response_header = oss.str();
        }

		int	_send(const char *a, int socket, size_t length)
		{
			i = 0;
			i = send(socket, a, length, 0);
			return i;
		}
    private :
		void	set_error(int status)
		{
			if (status == 400)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>400 Bad Request</title>\n  </head>\n  <body>\n    <h1>Bad Request</h1>\n    <p>The server could not understand your request.</p>\n  </body>\n</html>\n";
			else if (status == 401)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>401 Unauthorized</title>\n  </head>\n  <body>\n    <h1>Unauthorized</h1>\n    <p>You are not authorized to access this resource.</p>\n  </body>\n</html>\n";
			else if (status == 403)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>403 Forbidden</title>\n  </head>\n  <body>\n    <h1>Forbidden</h1>\n    <p>You do not have permission to access this resource.</p>\n  </body>\n</html>\n";
			else if (status == 404)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>404 Not Found</title>\n  </head>\n  <body>\n    <h1>Not Found</h1>\n    <p>The requested resource could not be found on the server.</p>\n  </body>\n</html>\n";
			else if (status == 405)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>405 Method Not Allowed</title>\n  </head>\n  <body>\n    <h1>Method Not Allowed</h1>\n    <p>The method specified in the request is not allowed for the resource.</p>\n  </body>\n</html>\n";
			else if (status == 409)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>409 Conflict</title>\n  </head>\n  <body>\n    <h1>Conflict</h1>\n    <p>The request could not be completed due to a conflict with the current state of the resource.</p>\n  </body>\n</html>\n";
			else if (status == 413)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>413 Payload Too Large</title>\n  </head>\n  <body>\n    <h1>Payload Too Large</h1>\n    <p>The server refused to process the request because the request payload is larger than the server is willing or able to process.</p>\n  </body>\n</html>\n";
			else if (status == 414)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>414 URI Too Long</title>\n  </head>\n  <body>\n    <h1>URI Too Long</h1>\n    <p>The server refused to process the request because the request-target is longer than the server is willing to interpret.</p>\n  </body>\n</html>\n";
			else if (status == 500)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>500 Internal Server Error</title>\n  </head>\n  <body>\n    <h1>Internal Server Error</h1>\n    <p>An error occurred on the server.</p>\n  </body>\n</html>\n";
			else if (status == 501)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>501 Not Implemented</title>\n  </head>\n  <body>\n    <h1>Not Implemented</h1>\n    <p>The server does not support the functionality required to fulfill the request.</p>\n  </body>\n</html>\n";
			else if (status == 502)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>502 Bad Gateway</title>\n  </head>\n  <body>\n    <h1>Bad Gateway</h1>\n    <p>The server received an invalid response from the upstream server while trying to fulfill the request.</p>\n  </body>\n</html>\n";
			else if (status == 201)
				dir_body = "<!DOCTYPE html>\n<html>\n  <head>\n    <title>201 Created</title>\n  </head>\n  <body>\n    <h1>Created</h1>\n    <p>The request has been fulfilled and a new resource has been created.</p>\n  </body>\n</html>\n";
		}
	
		void	setDirheader()
		{
			DIR *dire;
			dire = opendir(file_path.c_str());
			struct dirent *ent;
			std::ostringstream oss;
			oss << "<ul>\n";
			while ((ent = readdir(dire)) != NULL) {
				oss << "  <li><a href=\"" << ent->d_name << "\">" << ent->d_name << "</a></li>\n";
			}
			oss << "</ul>\n";
			this->dir_body = oss.str();
			closedir(dire);
		}

		void	get_path(int status, std::string &error_page)
		{
			if (status == 400)
				this->file_path = error_page + "400.html";
			if (status == 403)
				this->file_path = error_page + "403.html";
			if (status == 404)
				this->file_path = error_page + "404.html";
			if (status == 405)
				this->file_path = error_page + "405.html";
			if (status == 409)
				this->file_path = error_page + "409.html";
			if (status == 413)
				this->file_path = error_page + "413.html";
			if (status == 414)
				this->file_path = error_page + "414.html";
			if (status == 500)
				this->file_path = error_page + "500.html";
			if (status == 501)
				this->file_path = error_page + "501.html";
			if (status == 502)
				this->file_path = error_page + "502.html";
		}

		std::string check_MIME(std::string &file, bool dir)
		{
			if (dir)
				return "text/html";
			if (file.find_last_of('.') != std::string::npos)
			{
				std::string tmp = file.substr(file.find_last_of('.'), file.length());
				if (tmp == ".html")
					return "text/html";
				if (tmp == ".css")
					return "text/css";
				if (tmp == ".js")
					return "application/javascript";
				if (tmp == ".png")
					return "image/png";
				if (tmp == ".jpg")
					return "image/jpg";
				if (tmp == ".jpeg")
					return "image/jpeg";
				if (tmp == ".gif")
					return "image/gif";
				if (tmp == ".pdf")
					return "application/pdf";
				if (tmp == ".mp4")
					return "video/mp4";
				if (tmp == ".ico")
					return "image/x-icon";
				if (tmp == ".webp")
					return "image/webp";
				if (tmp == ".mp3")
					return "audio/mpeg";
			}
			return "text/plain";
		}


		std::string getCurrentDate() {
			std::time_t now = std::time(NULL);
			char buffer[80];
			std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
			return std::string(buffer);
		}

		std::string get_size(std::string _file)
		{
			std::ifstream file(_file, std::ios::binary);
			file.seekg(0, std::ios::end);
			std::streampos size = file.tellg();
			file.close();
			std::stringstream ss;
			ss << size;
			return ss.str();
		}

		std::string response_message(int status)
		{
			if (status == 201)
				return " 201 Created\r\n";
			if (status == 204)
				return " 204 No Content\r\n";
			if (status == 501)
				return " 501 Not Implemented\r\n";
			if (status == 400)
				return " 400 Bad Request\r\n";
			if (status == 414)
				return " 414 Request-URI Too Long\r\n";
			if (status == 413)
				return " 413 Request Entity Too Large\r\n";
			if (status == 404)
				return " 404 Not Found\r\n";
			if (status == 301)
				return " 301 Moved Permanently\r\n";
			if (status == 405)
				return " 405 Method Not Allowed\r\n";
			if (status == 403)
				return " 403 Forbidden\r\n";
			if (status == 409)
				return " 409 Conflict\r\n";
			if (status == 500)
				return " 500 Internal Server Error\r\n";
			if (status == 502)
				return " 502 Bad Gateway\r\n";
			return " 200 OK\r\n";
		}
};