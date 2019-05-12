#ifndef __CSAPP_H__
#define __CSAPP_H__

#include "config.h"
#include "w_error.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Default file permissions are DEF_MODE & ~DEF_UMASK */
/* $begin createmasks */
#define DEF_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
#define DEF_UMASK S_IWGRP | S_IWOTH
/* $end createmasks */

/* External variables */
extern int h_errno; /* Defined by BIND for DNS errors */

/* Directory wrappers */
DIR* Opendir(const char* name);
struct dirent* Readdir(DIR* dirp);
int Closedir(DIR* dirp);

/* Memory mapping wrappers */
void* Mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset);
void Munmap(void* start, size_t length);

/* Standard I/O wrappers */
void Fclose(FILE* fp);
FILE* Fdopen(int fd, const char* type);
char* Fgets(char* ptr, int n, FILE* stream);
FILE* Fopen(const char* filename, const char* mode);
void Fputs(const char* ptr, FILE* stream);
size_t Fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
void Fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);

/* Dynamic storage allocation wrappers */
void* Malloc(size_t size);
void* Realloc(void* ptr, size_t size);
void* Calloc(size_t nmemb, size_t size);
void Free(void* ptr);

#endif