#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::string path = "/data/open/lk";
    std::vector<std::string> pathComponents;
    std::stringstream ss(path);

    std::string component;
    std::getline(ss, component, '/');
    std::string tmp = "\0";
    pathComponents.push_back("/" + component);
    while (std::getline(ss, component, '/'))
    {
        pathComponents.push_back(tmp + "/" + component);
        tmp += "/" + component;
    }

    // Print the path components
    for (std::vector<std::string>::const_iterator it = pathComponents.begin(); it != pathComponents.end(); ++it) {
        std::cout << *it << std::endl;
    }

    return 0;
}

int main(int argc, char const *argv[]) {
    int server_fd1, server_fd2, new_socket, max_sd, activity;
    struct sockaddr_in address1, address2;
    int addrlen = sizeof(address1);
    char buffer[1024] = {0};
    fd_set readfds;

    // create server sockets
    if ((server_fd1 = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if ((server_fd2 = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // bind sockets to ports
    address1.sin_family = AF_INET;
    address1.sin_addr.s_addr = INADDR_ANY;
    address1.sin_port = htons(PORT1);
    if (bind(server_fd1, (struct sockaddr *)&address1, sizeof(address1))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    address2.sin_family = AF_INET;
    address2.sin_addr.s_addr = INADDR_ANY;
    address2.sin_port = htons(PORT2);
    if (bind(server_fd2, (struct sockaddr *)&address2, sizeof(address2))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if (listen(server_fd1, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd2, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // clear the socket set
        FD_ZERO(&readfds);

        // add server sockets to set
        FD_SET(server_fd1, &readfds);
        FD_SET(server_fd2, &readfds);
        max_sd = (server_fd1 > server_fd2) ? server_fd1 : server_fd2;

        // add connected client sockets to set
        // ...

        // wait for activity on any socket
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno!=EINTR)) {
            printf("select error");
        }

        // handle activity on server socket 1
        if (FD_ISSET(server_fd1, &readfds)) {
            if ((new_socket = accept(server_fd1, (struct sockaddr *)&address1, (socklen_t*)&addrlen))<0) {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }

            // handle new connection
            // ...
        }

        // handle activity on server socket 2
        if (FD_ISSET(server_fd2, &readfds)) {
            if ((new_socket = accept(server_fd2, (struct sockaddr *)&address2, (socklen_t*)&addrlen))<0) {
                perror("accept failed");
                exit(EXIT_FAILURE);
            }

            // handle new connection
            // ...
        }

        // handle activity on client sockets