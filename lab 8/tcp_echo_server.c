#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 12345
#define BUF_SIZE 1024

void sigchld_handler(int signo) {
    (void)signo;
    while (waitpid(-1, NULL, WNOHANG) > 0) {}
}

void handle_client(int connfd) {
    char buf[BUF_SIZE];
    ssize_t n;

    for (;;) {
        n = recv(connfd, buf, BUF_SIZE, 0);
        if (n <= 0)
            break;
        send(connfd, buf, n, 0);
    }

    close(connfd);
    exit(0);
}

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clilen;

    signal(SIGCHLD, sigchld_handler);

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

    printf("Echo server running on port %d\n", PORT);

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        if (connfd < 0)
            continue;

        pid_t pid = fork();
        if (pid < 0) {
            close(connfd);
            continue;
        } else if (pid == 0) {
            close(listenfd);
            handle_client(connfd);
        } else {
            close(connfd);
        }
    }

    close(listenfd);
    return 0;
}
