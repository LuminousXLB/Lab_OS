/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include <stdio.h>
#include <time.h>
#include "csapp.h"
#include "rio.h"

pid_t pid;

char* timestring() {
  static time_t curtime;
  curtime = time(NULL);
  char* ts = ctime(&curtime);
  ts[24] = '\0';
  return ts;
}

void echo(int connfd) {
  size_t n;
  char buf[MAXLINE];
  rio_t rio;

  Rio_readinitb(&rio, connfd);
  while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    printf("[%s] %d: Received %d bytes\n", timestring(), (int)pid, (int)n);
    Rio_writen(connfd, buf, n);
  }
}

int main(int argc, char const* argv[]) {
  pid = getpid();
  printf("[%s] %d: Servant Startup\n", timestring(), (int)pid);
  printf("[%s] %d: %s connect to me\n", timestring(), (int)pid, argv[2]);

  int connfd;
  sscanf(argv[1], "%d", &connfd);
  echo(connfd);
  Close(connfd);

  printf("[%s] %d: Servant Exit\n", timestring(), (int)pid);

  return 0;
}