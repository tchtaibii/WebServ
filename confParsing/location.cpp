#include "parse.hpp"

std::vector<std::string> &location::get_method()
{
    return this->method;
}

void location::set_method(const std::vector<std::string> &b)
{
    this->method = b;
}

std::string const &location::get_root() const
{
    return this->root;
}

void location::set_root(const std::string &b)
{
    this->root = b;
}

std::string const &location::get_autoindex() const
{
    return this->autoindex;
}

void location::set_autoindex(const std::string &b)
{
    this->autoindex = b;
}

std::string const &location::get_default() const
{
    return this->_default;
}

void location::set_default(const std::string &b)
{
    this->_default = b;
}

std::string const &location::get_upload() const
{
    return this->upload;
}

void location::set_upload(const std::string &b)
{
    this->upload = b;
}

std::map<std::string, std::string> &location::get_redirect()
{
    return this->redirect;
}

void location::set_redirect(const std::map<std::string, std::string> &b)
{
    this->redirect = b;
}
