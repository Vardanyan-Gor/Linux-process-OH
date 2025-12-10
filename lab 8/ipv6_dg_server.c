#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in6 servaddr, cliaddr;
    char buffer[BUF_SIZE];
    socklen_t len = sizeof(cliaddr);

    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sockfd < 0) exit(1);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any;
    servaddr.sin6_port = htons(12345);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        close(sockfd);
        exit(1);
    }

    while (1) {
        int n = recvfrom(sockfd, buffer, BUF_SIZE, 0,
                         (struct sockaddr*)&cliaddr, &len);
        if (n <= 0) continue;

        for (int i = 0; i < n; i++)
            buffer[i] = toupper(buffer[i]);

        sendto(sockfd, buffer, n, 0,
               (struct sockaddr*)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}
