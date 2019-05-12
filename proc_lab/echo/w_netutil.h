#ifndef __W_NETUTIL_H__
#define __W_NETUTIL_H__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#include "w_error.h"

/* Protocol independent wrappers */
void Getaddrinfo(const char* node, const char* service, const struct addrinfo* hints, struct addrinfo** res);
void Getnameinfo(const struct sockaddr* sa, socklen_t salen, char* host, size_t hostlen, char* serv, size_t servlen, int flags);
void Freeaddrinfo(struct addrinfo* res);
void Inet_ntop(int af, const void* src, char* dst, socklen_t size);
void Inet_pton(int af, const char* src, void* dst);

/* DNS wrappers */
struct hostent* Gethostbyname(const char* name);
struct hostent* Gethostbyaddr(const char* addr, int len, int type);

#endif