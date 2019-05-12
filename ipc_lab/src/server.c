#include "w_procmg.h"
#include "w_signal.h"
#include "w_socket.h"

// SIGCHLD handler
void sigchld_handler(int sig)
{
    // continously reap terminated child until there's no zombie
    while (waitpid(-1, 0, WNOHANG) > 0)
        ;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    /* Set SIGCHLD handler */
    Signal(SIGCHLD, sigchld_handler);

    /* Open listen file descriptor */
    int listenfd = Open_listenfd(argv[1]);

    /* Waiting for connection request */
    int connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);

        if (Fork() == 0) {
            /* Child closes its listening socket */
            Close(listenfd);

            /* Executable file path */
            char executable[] = "./servant";

            /* Extract client socket address */
            struct sockaddr_in* sin = (struct sockaddr_in*)&clientaddr;
            char* ip = inet_ntoa(sin->sin_addr);
            char addr_info[24];
            sprintf(addr_info, "%s:%u", ip, sin->sin_port);

            /* Prepare connect socket file descriptor */
            char fd_info[6];
            sprintf(fd_info, "%d", connfd);

            /* Build args array */
            char* args[4] = { executable, fd_info, addr_info, NULL };

            /* Execut servant process */
            Execve(args[0], args, environ);
        } else {
            /* Parent closes connected socket (important!) */
            Close(connfd);
        }
    }
}