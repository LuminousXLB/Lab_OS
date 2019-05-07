#ifndef __TSH_ERRHANDLER__
#define __TSH_ERRHANDLER__

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

void unix_error(char *msg);
void app_error(char *msg);

/*
 * unix_error - unix-style error routine
 */
void unix_error(const char *msg) {
  fprintf(stdout, "%s: %s\n", msg, strerror(errno));
  exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(const char *msg) {
  fprintf(stdout, "%s\n", msg);
  exit(1);
}

#endif