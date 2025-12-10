#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

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
    ssize_t n;

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

    while (fgets(sendbuf, BUF_SIZE, stdin) != NULL) {
        size_t len = strlen(sendbuf);
        if (send(sockfd, sendbuf, len, 0) != (ssize_t)len) {
            perror("send");
            break;
        }

        n = recv(sockfd, recvbuf, BUF_SIZE - 1, 0);
        if (n <= 0) {
            break;
        }
        recvbuf[n] = '\0';
        fputs(recvbuf, stdout);
    }

    close(sockfd);
    return 0;
}
