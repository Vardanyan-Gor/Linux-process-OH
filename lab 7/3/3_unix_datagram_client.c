#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_SOCKET_PATH "/tmp/unix_datagram_server_socket"
#define CLIENT_SOCKET_PATH "/tmp/unix_datagram_client_socket"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    char buffer[BUFFER_SIZE];
    const char *message;
    ssize_t bytes_sent;
    ssize_t bytes_received;
    socklen_t server_len;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s 'message'\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    message = argv[1];

    client_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&client_addr, 0, sizeof(struct sockaddr_un));
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, CLIENT_SOCKET_PATH, sizeof(client_addr.sun_path) - 1);

    unlink(CLIENT_SOCKET_PATH);

    if (bind(client_fd, (struct sockaddr *)&client_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    server_len = sizeof(struct sockaddr_un);

    printf("Client sends: %s\n", message);

    bytes_sent = sendto(client_fd, message, strlen(message), 0,
                        (struct sockaddr *)&server_addr, server_len);
    if (bytes_sent == -1) {
        perror("sendto");
        close(client_fd);
        unlink(CLIENT_SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    bytes_received = recvfrom(client_fd, buffer, BUFFER_SIZE - 1, 0, NULL, NULL);
    if (bytes_received == -1) {
        perror("recvfrom");
        close(client_fd);
        unlink(CLIENT_SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    printf("Client received: %s\n", buffer);

    close(client_fd);
    unlink(CLIENT_SOCKET_PATH);
    return 0;
}
