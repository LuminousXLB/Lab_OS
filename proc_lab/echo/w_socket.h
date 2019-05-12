#ifndef __W_SOCKET_H__
#define __W_SOCKET_H__

#include <sys/socket.h>

#include "w_error.h"
#include "w_netutil.h"
#include "w_rio.h"
#include "w_unixio.h"

#define LISTENQ 1024 /* Second argument to listen() */

/* Simplifies calls to bind(), connect(), and accept() */
/* $begin sockaddrdef */
typedef struct sockaddr SA;
/* $end sockaddrdef */

/* Sockets interface wrappers */
int Socket(int domain, int type, int protocol);
void Setsockopt(int s, int level, int optname, const void* optval, int optlen);
void Bind(int sockfd, struct sockaddr* my_addr, int addrlen);
void Listen(int s, int backlog);
int Accept(int s, struct sockaddr* addr, socklen_t* addrlen);
void Connect(int sockfd, struct sockaddr* serv_addr, int addrlen);

/* Reentrant protocol-independent client/server helpers */
int open_clientfd(char* hostname, char* port);
int open_listenfd(char* port);

/* Wrappers for reentrant protocol-independent client/server helpers */
int Open_clientfd(char* hostname, char* port);
int Open_listenfd(char* port);

#endif