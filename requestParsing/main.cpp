#include "parsingRequest.hpp"
int main()
{
    // Define the HTTP request string with body
    std::string request_str =
        "POST / HTTP/1.1\r\n"
        "Transfer-Encoding: chunked\r\n"
        "User-Agent: PostmanRuntime/7.31.0\r\n"
        "Accept: */*\r\n"
        "Postman-Token: 751e4462-8145-4051-9123-fb6a2674c9cd\r\n"
        "Host: localhost:1337\r\n"
        "Accept-Encoding: gzip, deflate, br\r\n"
        "Connection: keep-alive\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "6dd\r\n"
        "server {\r\n"
        "    port = 1337\r\n"
        "    host = 127.0.0.1\r\n"
        "    server_name = server1\r\n"
        "    body_size_limit = 2000\r\n"
        "    error_page = ./eroor-pages\r\n"
        "    cgi = php /Users/mbouhaba/.brew/bin/php-cgi\r\n"
        "    cgi = python /usr/local/bin/python3\r\n"
        "\r\n"
        "    location = / [\r\n"
        "        method = GET POST DELETE\r\n"
        "        root = /Users/maryam/Desktop/\r\n"
        "        autoindex = on\r\n"
        "        default = index.html\r\n"
        "        upload = /uploads\r\n"
        "    ]\r\n"
        "\r\n"
        "    location = /Data [\r\n"
        "        method = GET POST\r\n"
        "        root = /goinfre/issam/\r\n"
        "        autoindex = on\r\n"
        "        default = index.html\r\n"
        "        upload = /uploads\r\n"
        "    ]\r\n"
        "\r\n"
        "    location = /Data/project [\r\n"
        "        method = GET POST\r\n"
        "        root = /goinfre/issam/\r\n"
        "        autoindex = on\r\n"
        "        default = index.html\r\n"
        "        upload = /uploads\r\n"
        "    ]\r\n"
        "\r\n"
        "    location = /Project [\r\n"
        "        method = GET POST\r\n"
        "        root = /Users/issam/Desktop/webserv/\r\n"
        "        autoindex = off\r\n"
        "        default = index.html\r\n"
        "        upload = /uploads\r\n"
        "    ]\r\n"
        "}\r\n"
        "\r\n"
        "server {\r\n"
        "    port = 8000\r\n"
        "    host = 127.0.0.1\r\n"
        "    server_name = server2\r\n"
        "    error_page = ./eroor-pages\r\n"
        "    body_size_limit = 5\r\n"
        "    cgi = python /usr/bin/python\r\n"
        "\r\n"
        "    location = / [\r\n"
        "        method = GET POST DELETE\r\n"
        "        root = /Users/issam/Desktop/\r\n"
        "        autoindex = off\r\n"
        "        default = /index.html\r\n"
        "        upload = /uploads\r\n"
        "        redirect = 301 https://github.com/\r\n"
        "\r\n"
        "0\r\n";
    // Parse the HTTP request string into an HttpRequest struct
    ws::HttpRequest req = ws::parse_http_request(request_str.c_str());
    // Print the request method, path, version, headers, and body
    std::cout << "Method: " << req.method << std::endl;
    std::cout << "Path: " << req.path << std::endl;
    std::cout << "Version: " << req.version << std::endl;
    std::cout << "Headers:" << std::endl;
    for (const auto &header : req.headers)
    {
        std::cout << "  " << header.first << ": " << header.second << std::endl;
    }
    std::cout << "Body:\n"
              << req.body << std::endl;
    return 0;
}