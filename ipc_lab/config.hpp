#ifndef __TSH_CONFIG__
#define __TSH_CONFIG__

/* Misc manifest constants */
#define MAXLINE 1024   /* max line size */
#define MAXARGS 128    /* max args on a command line */
#define MAXJID 1 << 16 /* max job ID */
#define MAXJOBS 16     /* max jobs at any point in time */

int verbose = 0;

#endif