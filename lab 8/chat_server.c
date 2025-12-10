#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 12345
#define MAX_CLIENTS  FD_SETSIZE
#define BUF_SIZE     1024

int main(void) {
    int listenfd, connfd;
    int client_socks[MAX_CLIENTS];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;
    fd_set allset, rset;
    int maxfd, i, nready;
    char buf[BUF_SIZE];

    for (i = 0; i < MAX_CLIENTS; i++)
        client_socks[i] = -1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        close(listenfd);
        exit(1);
    }

    if (listen(listenfd, 10) < 0) {
        perror("listen");
        close(listenfd);
        exit(1);
    }

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    maxfd = listenfd;

    printf("Chat server running on port %d\n", PORT);

    for (;;) {
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
        if (nready < 0) {
            perror("select");
            break;
        }

        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
            if (connfd >= 0) {
                for (i = 0; i < MAX_CLIENTS; i++) {
                    if (client_socks[i] < 0) {
                        client_socks[i] = connfd;
                        FD_SET(connfd, &allset);
                        if (connfd > maxfd)
                            maxfd = connfd;
                        break;
                    }
                }
                if (i == MAX_CLIENTS) {
                    close(connfd);
                }
            }
            if (--nready <= 0)
                continue;
        }

        for (i = 0; i < MAX_CLIENTS; i++) {
            int sock = client_socks[i];
            if (sock < 0)
                continue;

            if (FD_ISSET(sock, &rset)) {
                ssize_t n = recv(sock, buf, BUF_SIZE, 0);
                if (n <= 0) {
                    close(sock);
                    FD_CLR(sock, &allset);
                    client_socks[i] = -1;
                } else {
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int other = client_socks[j];
                        if (other >= 0 && other != sock) {
                            ssize_t sent = 0;
                            while (sent < n) {
                                ssize_t m = send(other, buf + sent, n - sent, 0);
                                if (m <= 0) break;
                                sent += m;
                            }
                        }
                    }
                }
                if (--nready <= 0)
                    break;
            }
        }
    }

    close(listenfd);
    return 0;
}
