#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_SOCKET_PATH "/tmp/unix_datagram_server_socket"
#define BUFFER_SIZE 1024

int main() {
    int server_fd;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    unlink(SERVER_SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Datagram server is running on %s\n", SERVER_SOCKET_PATH);

    while (1) {
        client_len = sizeof(struct sockaddr_un);
        bytes_read = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                              (struct sockaddr *)&client_addr, &client_len);
        if (bytes_read == -1) {
            perror("recvfrom");
            continue;
        }

        buffer[bytes_read] = '\0';
        printf("Server received: %s\n", buffer);

        if (sendto(server_fd, buffer, bytes_read, 0,
                   (struct sockaddr *)&client_addr, client_len) == -1) {
            perror("sendto");
        }
    }

    close(server_fd);
    unlink(SERVER_SOCKET_PATH);
    return 0;
}
