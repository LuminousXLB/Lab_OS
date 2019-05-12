/* msgcom.h */
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGKEY 5678

struct msgtype
{
    long mtype;
    int text;
};
