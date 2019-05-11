#include <arpa/inet.h>
#include "csapp.h"

/* SIGCHLD handler */
void sigchld_handler(int sig) {
  while (waitpid(-1, 0, WNOHANG) > 0)
    ;
  return;
}

int main(int argc, char** argv) {
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }

  Signal(SIGCHLD, sigchld_handler);
  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    connfd = Accept(listenfd, (SA*)&clientaddr, &clientlen);

    if (Fork() == 0) {
      /* Child closes its listening socket */
      Close(listenfd);

      struct sockaddr_in* sin = (struct sockaddr_in*)&clientaddr;
      char* ip = inet_ntoa(sin->sin_addr);

      char executable[] = "./servant";
      char fd_info[6] = {'\0'};
      char addr_info[24] = {'\0'};
      sprintf(fd_info, "%d", connfd);
      sprintf(addr_info, "%s:%u", ip, sin->sin_port);

      char* args[4] = {executable, fd_info, addr_info, NULL};

      Execve(args[0], args, environ);
    } else {
      /* Parent closes connected socket (important!) */
      Close(connfd);
    }
  }
}