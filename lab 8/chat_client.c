#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 12345
#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char sendbuf[BUF_SIZE];
    char recvbuf[BUF_SIZE];
    fd_set rset;
    int maxfd;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    printf("Connected to chat. Type messages:\n");

    for (;;) {
        FD_ZERO(&rset);
        FD_SET(sockfd, &rset);
        FD_SET(STDIN_FILENO, &rset);
        maxfd = (sockfd > STDIN_FILENO ? sockfd : STDIN_FILENO);

        if (select(maxfd + 1, &rset, NULL, NULL, NULL) < 0) {
            perror("select");
            break;
        }

        if (FD_ISSET(sockfd, &rset)) {
            ssize_t n = recv(sockfd, recvbuf, BUF_SIZE - 1, 0);
            if (n <= 0) {
                printf("Server closed connection.\n");
                break;
            }
            recvbuf[n] = '\0';
            fputs(recvbuf, stdout);
            fflush(stdout);
        }

        if (FD_ISSET(STDIN_FILENO, &rset)) {
            if (fgets(sendbuf, BUF_SIZE, stdin) == NULL) {
                shutdown(sockfd, SHUT_WR);
                continue;
            }
            size_t len = strlen(sendbuf);
            if (len > 0) {
                ssize_t sent = 0;
                while (sent < (ssize_t)len) {
                    ssize_t m = send(sockfd, sendbuf + sent, len - sent, 0);
                    if (m <= 0) {
                        goto done;
                    }
                    sent += m;
                }
            }
        }
    }

done:
    close(sockfd);
    return 0;
}
