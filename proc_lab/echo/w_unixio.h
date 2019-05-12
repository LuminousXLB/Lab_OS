#ifndef __W_UNIXIO_H__
#define __W_UNIXIO_H__

#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#include "w_error.h"

/* Unix I/O wrappers */
int Open(const char* pathname, int flags, mode_t mode);
ssize_t Read(int fd, void* buf, size_t count);
ssize_t Write(int fd, const void* buf, size_t count);
off_t Lseek(int fildes, off_t offset, int whence);
void Close(int fd);

#endif