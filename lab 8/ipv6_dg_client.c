#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) exit(1);

    int sockfd;
    struct sockaddr_in6 servaddr;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sockfd < 0) exit(1);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_port = htons(12345);

    if (inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr) <= 0) exit(1);

    sendto(sockfd, argv[2], strlen(argv[2]), 0,
           (struct sockaddr*)&servaddr, sizeof(servaddr));

    int n = recvfrom(sockfd, buffer, BUF_SIZE, 0, NULL, NULL);
    buffer[n] = '\0';

    printf("%s\n", buffer);

    close(sockfd);
    return 0;
}
