#ifndef __W_PROCMG_H__
#define __W_PROCMG_H__

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "w_error.h"

extern char** environ; /* Defined by libc */

/* Process control wrappers */
pid_t Fork(void);
void Execve(const char* filename, char* const argv[], char* const envp[]);
pid_t Wait(int* status);
pid_t Waitpid(pid_t pid, int* iptr, int options);
void Kill(pid_t pid, int signum);
unsigned int Sleep(unsigned int secs);
void Pause(void);
unsigned int Alarm(unsigned int seconds);
void Setpgid(pid_t pid, pid_t pgid);
pid_t Getpgrp();

#endif