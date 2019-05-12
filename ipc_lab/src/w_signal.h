#ifndef __W_SIGNAL_H__
#define __W_SIGNAL_H__

#include <signal.h>
#include <unistd.h>
#include "w_error.h"

/* Signal wrappers */
typedef void handler_t(int);

handler_t* Signal(int signum, handler_t* handler);

/* Sio (Signal-safe I/O) routines */
ssize_t sio_puts(char s[]);
ssize_t sio_putl(long v);
void sio_error(char s[]);

/* Sio wrappers */
ssize_t Sio_puts(char s[]);
ssize_t Sio_putl(long v);
void Sio_error(char s[]);

#endif