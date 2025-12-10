#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/select.h>

#define SOCKET_PATH "/tmp/chat_stream_socket"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS  FD_SETSIZE

int main() {
    int server_fd;
    struct sockaddr_un addr;
    int client_fds[MAX_CLIENTS];
    int i;
    fd_set readfds;
    int max_fd;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = -1;
    }

    printf("Chat server is running\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        for (i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] >= 0) {
                FD_SET(client_fds[i], &readfds);
                if (client_fds[i] > max_fd) {
                    max_fd = client_fds[i];
                }
            }
        }

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }

        if (FD_ISSET(server_fd, &readfds)) {
            int new_fd = accept(server_fd, NULL, NULL);
            if (new_fd == -1) {
                perror("accept");
            } else {
                int placed = 0;
                for (i = 0; i < MAX_CLIENTS; i++) {
                    if (client_fds[i] < 0) {
                        client_fds[i] = new_fd;
                        placed = 1;
                        printf("New client connected\n");
                        break;
                    }
                }
                if (!placed) {
                    printf("Too many clients, rejecting\n");
                    close(new_fd);
                }
            }
        }

        for (i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];
            if (fd >= 0 && FD_ISSET(fd, &readfds)) {
                ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE);
                if (bytes_read <= 0) {
                    if (bytes_read == 0) {
                        printf("Client disconnected\n");
                    } else {
                        perror("read");
                    }
                    close(fd);
                    client_fds[i] = -1;
                } else {
                    int j;
                    for (j = 0; j < MAX_CLIENTS; j++) {
                        int other_fd = client_fds[j];
                        if (other_fd >= 0 && other_fd != fd) {
                            ssize_t bytes_sent = write(other_fd, buffer, bytes_read);
                            if (bytes_sent == -1) {
                                perror("write");
                            }
                        }
                    }
                }
            }
        }
    }

    for (i = 0; i < MAX_CLIENTS; i++) {
        if (client_fds[i] >= 0) {
            close(client_fds[i]);
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
