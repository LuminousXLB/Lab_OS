#include "w_socket.h"
#define MAXLINE 8192

char* Fgets(char* ptr, int n, FILE* stream)
{
    char* rptr;

    if (((rptr = fgets(ptr, n, stream)) == NULL) && ferror(stream))
        app_error("Fgets error");

    return rptr;
}

void Fputs(const char* ptr, FILE* stream)
{
    if (fputs(ptr, stream) == EOF)
        unix_error("Fputs error");
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }

    rio_t rio;
    int clientfd = Open_clientfd(argv[1], argv[2]);
    Rio_readinitb(&rio, clientfd);

    char buf[MAXLINE];
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        Rio_writen(clientfd, buf, strlen(buf));
        Rio_readlineb(&rio, buf, MAXLINE);
        Fputs(buf, stdout);
    }
    
    Close(clientfd);
    exit(0);
}