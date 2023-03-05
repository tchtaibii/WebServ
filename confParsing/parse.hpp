#pragma once
#include <iostream>
#include <map>
#include <vector>

class	location
{
	private :
        std::vector<std::string> method;
        std::string root;
        std::string autoindex;
        std::string _default;
        std::string	upload;
        std::map<std::string, std::string> redirect;

	public :
		int											flg;
		std::vector<std::string> &					get_method() ;
		void					 					set_method(const std::vector<std::string> & b);
		std::string const &							get_root() const;
		void 										set_root(const std::string &b);
		std::string	const &							get_autoindex() const;
		void										set_autoindex(const std::string & b);
		std::string	const &							get_default() const;
		void										set_default(const std::string & b);
		std::string	const &							get_upload() const;
		void										set_upload(const std::string &b);
		std::map<std::string, std::string> &		get_redirect();
		void 										set_redirect(const std::map<std::string, std::string> &b);
};

class server
{
    private :
        std::string 					    		port;
		int											socket;
        std::string 								host;
        std::string 								server_name;
        std::string                         		body_size;
        std::string                         		error_page;
        std::map<std::string, std::string>  		cgi;
		std::map<std::string, location>				_location;
    public :
		int											flg;
        std::string const &  						get_port() const ;
		int const &  								getSocket() const ;
		void		  								setSocket(const int &sock);
        void    									set_port(const std::string &p);
        std::string const &							get_host() const; 
		void										set_host(const std::string &host);
		std::string const &							get_server_name() const ;
		void 										set_server_name(const std::string &server_name);
        std::string  const &      					get_body_size() const ;
        void            							set_body_size(const std::string &a);
        std::string  const  &  						get_error_page() const ;
        void     									set_error_page(const std::string & e);
		std::map<std::string, std::string>  &		get_cgi();
		void 										set_cgi(const std::map<std::string, std::string> & c);
		std::map<std::string, location>  &			get_location() ;
		void										set_location(const std::map<std::string, location> &a);
};