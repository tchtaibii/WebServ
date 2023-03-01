#include "socket.hpp"
#include <string.h>

int main() {
    const int NUM_SOCKETS = 5;
    int ports[3] = {8000, 3621, 8745};
    ws::Socket sockets[3];

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            // error handling
        } else if (pid == 0) {
            sockets[i].setPort(ports[i]);  // child process
            sockets[i].start();
            exit(0);
        }
    }

    // wait for child processes to exit
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    return 0;
}