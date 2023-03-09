#include "parsingRequest.hpp"
int main()
{
    // Define the HTTP request string with body
    std::string request_str =
    "POST / HTTP/1.1\r\n"
    "User-Agent: PostmanRuntime/7.31.0\r\n"
    "Accept: */*\r\n"
    "Postman-Token: 98f14b5e-679f-4299-b0b3-b6a520221d81\r\n"
    "Host: localhost:1337\r\n"
    "Accept-Encoding: gzip, deflate, br\r\n"
    "Connection: keep-alive\r\n"
    "Content-Length: 286\r\n"
    "Content-Type: application/octet-stream\r\n"
    "\r\n"
    "FLAGS = -Wall -Wextra -Werror\r\n"
    "SRC = confParsing/*.cpp main.cpp\r\n"
    "CC = c++\r\n"
    "\r\n"
    "all : server \r\n"
    "\r\n"
    "server : $(SRC_CLIENT)\r\n"
    "        @$(CC) $(FLAGS) $(SRC) -o webserv\r\n"
    "        @tput setaf 2; echo \"Server IS READY\"\r\n"
    "\r\n"
    "clean :\r\n"
    "        @rm -f client webserv\r\n"
    "        @tput setaf 1; echo \"CLEAN COMPLET\"\r\n"
    "\r\n"
    "fclean : clean\r\n"
    "\r\n"
    "re: fclean all";
    // Parse the HTTP request string into an HttpRequest struct
    ws::HttpRequest req = ws::parse_http_request(request_str.c_str());
    // Print the request method, path, version, headers, and body
    std::cout << "Method: " << req.method << std::endl;
    std::cout << "Path: " << req.path << std::endl;
    std::cout << "Version: " << req.version << std::endl;
    std::cout << "Headers:" << std::endl;
    for (const auto& header : req.headers)
    {
        std::cout << "  " << header.first << ": " << header.second << std::endl;
    }
    std::cout << "Body: " << req.body << std::endl;
    return 0;
}