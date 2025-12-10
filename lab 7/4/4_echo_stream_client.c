#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/echo_stream_socket"
#define BUFFER_SIZE 1024

int main() {
    int client_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_sent;
    ssize_t bytes_received;

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to echo server\n");

    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
        bytes_sent = write(client_fd, buffer, strlen(buffer));
        if (bytes_sent == -1) break;

        bytes_received = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_received <= 0) break;

        buffer[bytes_received] = '\0';
        printf("Echo: %s", buffer);
    }

    close(client_fd);
    return 0;
}
