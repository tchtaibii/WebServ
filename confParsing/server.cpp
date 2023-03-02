#include "parse.hpp"

std::string const &     server::get_port() const
{
    return this->port;
}

void                    server::set_port(const std::string &p)
{
    this->port = p;
}

std::string const &		server::get_host() const
{
    return this->host;
}

void	server::set_host(const std::string &host)
{
    this->host = host;
}

std::string const &  server::get_server_name() const
{
    return this->server_name;
}

void 								server::set_server_name(const std::string &server_name)
{
    this->server_name = server_name;
}

std::string  const &       server::get_body_size() const
{
    return this->body_size;
}

void            					server::set_body_size(const std::string &a)
{
    this->body_size = a;
}

std::string const &     server::get_error_page() const
{
    return this->error_page;
}

void     							server::set_error_page(const std::string & e)
{
    this->error_page = e;
}

std::map<std::string, std::string> &	server::get_cgi() 
{
    return this->cgi;
}

void 		server::set_cgi(const std::map<std::string, std::string> & c)
{
    this->cgi = c;
}

std::map<std::string, location>  &   server::get_location()
{
    return this->_location;
}

void		server::set_location(const std::map<std::string, location> &a)
{
    this->_location = a;
}
