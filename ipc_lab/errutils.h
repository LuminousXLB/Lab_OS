#ifndef __ERR_UTILS_H__
#define __ERR_UTILS_H__

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Our own error-handling functions */
void unix_error(char* msg);
void posix_error(int code, char* msg);
void dns_error(char* msg);
void gai_error(int code, char* msg);
void app_error(char* msg);

#endif