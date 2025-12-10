#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[BUF_SIZE];
    socklen_t len = sizeof(cliaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(12345);

    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind error");
        close(sockfd);
        exit(1);
    }

    printf("Server running on port 12345...\n");

    while (1) {
        int n = recvfrom(sockfd, buffer, BUF_SIZE, 0,
                         (struct sockaddr*)&cliaddr, &len);
        if (n < 0) continue;

        buffer[n] = '\0';

        for (int i = 0; i < n; i++)
            buffer[i] = toupper(buffer[i]);

        sendto(sockfd, buffer, n, 0,
               (struct sockaddr*)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}
