#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server-ip> <message>\n", argv[0]);
        exit(1);
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(12345);

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("inet_pton error");
        exit(1);
    }

    sendto(sockfd, argv[2], strlen(argv[2]), 0,
           (struct sockaddr*)&servaddr, sizeof(servaddr));

    int n = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
    buffer[n] = '\0';

    printf("Server reply: %s\n", buffer);

    close(sockfd);
    return 0;
}
