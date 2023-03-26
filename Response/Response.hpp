#pragma once
#include <dirent.h>
#include "../Request/tools.hpp"
#include "../cgi/cgi.hpp"

class response 
{
    public :
        bool		done;
        bool		first_time;
		std::string response_header;
		std::string file_path;
		std::string dir_body;
		std::string cgi_file_path;
		int			i;
		int			fd;
	
        void    set_header(std::string file, int status, ws::HttpRequest req, bool dir)
        {

			this->first_time = true;
			this->file_path = file;
			if (dir && status != 301 && status != 403)
				setDirheader();
			if (status != 200 && status != 301 && status != 204)
				get_path(status);
			if (check_extension2(file_path))
			{
				cgi c(file_path);
				c.exec();
				file_path = c.get_outfile_path();
			}
			std::ostringstream oss;
			oss << req.version + response_message(status);
			oss << "Date: " << getCurrentDate() << "\r\n";
			if (status != 204)
			{
				if (status == 301)
				{
					oss << "Location: " << file << "\r\n";
					oss << "Content-Length: " << '0' << "\r\n";
				}
				else
				{
					oss << "Content-Type: " <<  check_MIME(file_path, dir) << "\r\n";
					if (status != 209)
					{
						if (!dir || (dir && status == 403))
							oss << "Content-Length: " << get_size(file_path) << "\r\n";
						else
							oss << "Content-Length: " << dir_body.length() << "\r\n";
					}
				}
			}
			oss << "\r\n";
			this->response_header = oss.str();
			std::cout << response_header << std::endl;
        }

		int	_send(const char *a, int socket, size_t length)
		{
			i = 0;
			i = send(socket, a, length, 0);
			return i;
		}
    private :
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

		void	get_path(int status)
		{
			if (status == 400)
				this->file_path = "./Assets/Errors/400.html";
			if (status == 403)
				this->file_path = "./Assets/Errors/403.html";
			if (status == 404)
				this->file_path = "./Assets/Errors/404.html";
			if (status == 405)
				this->file_path = "./Assets/Errors/405.html";
			if (status == 409)
				this->file_path = "./Assets/Errors/409.html";
			if (status == 413)
				this->file_path = "./Assets/Errors/413.html";
			if (status == 414)
				this->file_path = "./Assets/Errors/414.html";
			if (status == 500)
				this->file_path = "./Assets/Errors/500.html";
			if (status == 501)
				this->file_path = "./Assets/Errors/501.html";
			if (status == 502)
				this->file_path = "./Assets/Errors/502.html";
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