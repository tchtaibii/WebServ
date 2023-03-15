#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

// Define a constant buffer size for reading data
const int BUFFER_SIZE = 1024;

// Read a line from a socket
std::string read_line(int socket)
{
    std::string line;
    char c = '\0';
    while (c != '\n')
    {
        recv(socket, &c, 1, 0);
        if (c != '\r')
        {
            line += c;
        }
    }
    return line;
}

// Read data in chunks from a socket
std::vector<char> read_chunked_data(int socket)
{
    std::vector<char> data;
    while (true)
    {
        // Read the chunk size
        std::string size_str = read_line(socket);
        int size = std::stoi(size_str, nullptr, 16);
        if (size == 0)
        {
            break;
        }
        // Read the chunk data
        std::vector<char> chunk_data(size);
        int bytes_read = 0;
        while (bytes_read < size)
        {
            int bytes_to_read = std::min(BUFFER_SIZE, size - bytes_read);
            bytes_read += recv(socket, &chunk_data[bytes_read], bytes_to_read, 0);
        }
        // Append the chunk data to the complete data
        data.insert(data.end(), chunk_data.begin(), chunk_data.end());
        // Read the CRLF sequence after the chunk data
        char crlf[2];
        recv(socket, crlf, 2, 0);
    }
    return data;
}

// Handle an HTTP request
void handle_request(int client_socket)
{
    // Read the request headers
    std::string headers_str;
    while (true)
    {
        std::string line = read_line(client_socket);
        if (line == "\r\n")
        {
            break;
        }
        headers_str += line + "\r\n";
    }
    // Check if the request is chunked
    bool is_chunked = false;
    if (headers_str.find("Transfer-Encoding: chunked") != std::string::npos)
    {
        is_chunked = true;
    }
    // Read the request data
    std::vector<char> data;
    if (is_chunked)
    {
        data = read_chunked_data(client_socket);
        // std::cout << data << std::endl;
    }
    else
    {
        char buffer[BUFFER_SIZE];
        int bytes_read = 0;
        while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)
        {
            data.insert(data.end(), buffer, buffer + bytes_read);
        }
    }
    std::cout << "okay" << std::endl;
    // Process the request
    std::string request_str(data.begin(), data.end());
    std::cout << "Received request: " << request_str << std::endl;
}

int main()
{
    // Create a socket and bind it to a port
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8000);
    server_address.sin_addr.s_addr = INADDR_ANY;
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    // Listen for incoming connections
    listen(server_socket, 5);
    // Accept incoming connections and handle requests
    while (true)
    {
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);
        handle_request(client_socket);
        close(client_socket);
    }
    close(server_socket);
    return 0;
}