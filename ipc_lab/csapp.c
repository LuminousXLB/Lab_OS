#include "csapp.h"

/*********************************
 * Wrappers for directory function
 *********************************/

DIR* Opendir(const char* name)
{
    DIR* dirp = opendir(name);

    if (!dirp)
        unix_error("opendir error");
    return dirp;
}

struct dirent* Readdir(DIR* dirp)
{
    struct dirent* dep;

    errno = 0;
    dep = readdir(dirp);
    if ((dep == NULL) && (errno != 0))
        unix_error("readdir error");
    return dep;
}

int Closedir(DIR* dirp)
{
    int rc;

    if ((rc = closedir(dirp)) < 0)
        unix_error("closedir error");
    return rc;
}

/***************************************
 * Wrappers for memory mapping functions
 ***************************************/
void* Mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset)
{
    void* ptr;

    if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void*)-1))
        unix_error("mmap error");
    return (ptr);
}

void Munmap(void* start, size_t length)
{
    if (munmap(start, length) < 0)
        unix_error("munmap error");
}

/***************************************************
 * Wrappers for dynamic storage allocation functions
 ***************************************************/

void* Malloc(size_t size)
{
    void* p;

    if ((p = malloc(size)) == NULL)
        unix_error("Malloc error");
    return p;
}

void* Realloc(void* ptr, size_t size)
{
    void* p;

    if ((p = realloc(ptr, size)) == NULL)
        unix_error("Realloc error");
    return p;
}

void* Calloc(size_t nmemb, size_t size)
{
    void* p;

    if ((p = calloc(nmemb, size)) == NULL)
        unix_error("Calloc error");
    return p;
}

void Free(void* ptr)
{
    free(ptr);
}

/******************************************
 * Wrappers for the Standard I/O functions.
 ******************************************/
void Fclose(FILE* fp)
{
    if (fclose(fp) != 0)
        unix_error("Fclose error");
}

FILE* Fdopen(int fd, const char* type)
{
    FILE* fp;

    if ((fp = fdopen(fd, type)) == NULL)
        unix_error("Fdopen error");

    return fp;
}



size_t Fread(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    size_t n;

    if (((n = fread(ptr, size, nmemb, stream)) < nmemb) && ferror(stream))
        unix_error("Fread error");
    return n;
}

void Fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    if (fwrite(ptr, size, nmemb, stream) < nmemb)
        unix_error("Fwrite error");
}
