#include "w_socket.h"

/****************************
 * Sockets interface wrappers
 ****************************/

int Socket(int domain, int type, int protocol)
{
    int rc;

    if ((rc = socket(domain, type, protocol)) < 0)
        unix_error("Socket error");
    return rc;
}

void Setsockopt(int s, int level, int optname, const void* optval, int optlen)
{
    int rc;

    if ((rc = setsockopt(s, level, optname, optval, optlen)) < 0)
        unix_error("Setsockopt error");
}

void Bind(int sockfd, struct sockaddr* my_addr, int addrlen)
{
    int rc;

    if ((rc = bind(sockfd, my_addr, addrlen)) < 0)
        unix_error("Bind error");
}

void Listen(int s, int backlog)
{
    int rc;

    if ((rc = listen(s, backlog)) < 0)
        unix_error("Listen error");
}

int Accept(int s, struct sockaddr* addr, socklen_t* addrlen)
{
    int rc;

    if ((rc = accept(s, addr, addrlen)) < 0)
        unix_error("Accept error");
    return rc;
}

void Connect(int sockfd, struct sockaddr* serv_addr, int addrlen)
{
    int rc;

    if ((rc = connect(sockfd, serv_addr, addrlen)) < 0)
        unix_error("Connect error");
}

/********************************
 * Client/server helper functions
 ********************************/
/*
 * open_clientfd - Open connection to server at <hostname, port> and
 *     return a socket descriptor ready for reading and writing. This
 *     function is reentrant and protocol-independent.
 *
 *     On error, returns:
 *       -2 for getaddrinfo error
 *       -1 with errno set for other errors.
 */
/* $begin open_clientfd */
int open_clientfd(char* hostname, char* port)
{

    /* Get a list of potential server addresses */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    /* Open a connection */
    hints.ai_socktype = SOCK_STREAM;
    /* ... using a numeric port arg. */
    hints.ai_flags = AI_NUMERICSERV;
    /* Recommended for connections */
    hints.ai_flags |= AI_ADDRCONFIG;

    int rc;
    struct addrinfo* listp;
    if ((rc = getaddrinfo(hostname, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n", hostname, port,
            gai_strerror(rc));
        return -2;
    }

    /* Walk the list for one that we can successfully connect to */
    int clientfd;
    struct addrinfo* p;
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            /* Socket failed, try the next */
            continue;
        }

        /* Connect to the server */
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) {
            /* Success */
            break;
        }

        if (close(clientfd) < 0) {
            /* Connect failed, try another */
            fprintf(stderr, "open_clientfd: close failed: %s\n", strerror(errno));
            return -1;
        }
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) {
        /* All connects failed */
        return -1;
    } else {
        /* The last connect succeeded */
        return clientfd;
    }
}
/* $end open_clientfd */

/*
 * open_listenfd - Open and return a listening socket on port. This
 *     function is reentrant and protocol-independent.
 *
 *     On error, returns:
 *       -2 for getaddrinfo error
 *       -1 with errno set for other errors.
 */
/* $begin open_listenfd */
int open_listenfd(char* port)
{
    /* Get a list of potential server addresses */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    /* Accept connections */
    hints.ai_socktype = SOCK_STREAM;
    /* ... on any IP address using port number */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;

    int rc;
    struct addrinfo* listp;
    if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
        return -2;
    }

    /* Walk the list for one that we can bind to */
    struct addrinfo* p;
    int listenfd, optval = 1;
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
            /* Socket failed, try the next */
            continue;
        }

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
            /* Success */
            break;
        }

        if (close(listenfd) < 0) { /* Bind failed, try the next */
            fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
            return -1;
        }
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) {
        /* No address worked */
        return -1;
    }

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0) {
        close(listenfd);
        return -1;
    }

    return listenfd;
}
/* $end open_listenfd */

/****************************************************
 * Wrappers for reentrant protocol-independent helpers
 ****************************************************/
int Open_clientfd(char* hostname, char* port)
{
    int rc;

    if ((rc = open_clientfd(hostname, port)) < 0)
        unix_error("Open_clientfd error");
    return rc;
}

int Open_listenfd(char* port)
{
    int rc;

    if ((rc = open_listenfd(port)) < 0)
        unix_error("Open_listenfd error");
    return rc;
}
