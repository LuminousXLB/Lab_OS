#ifndef __SYNC_H__
#define __SYNC_H__

#include <stdbool.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "errutils.h"

#define SERVANT_COUNT 4

struct semProducerConsumer {
    int available;
    int mutex;
    int shmid;
    bool* status;
};

void initAllocator(struct semProducerConsumer* spc);
int allocServant(struct semProducerConsumer* spc);
void freeServant(struct semProducerConsumer* spc, int i);

/* Semaphore */
union semun {
    int val; /* Value for SETVAL */
    struct semid_ds* buf; /* Buffer for IPC_STAT, IPC_SET */
    unsigned short* array; /* Array for GETALL, SETALL */
};

int semInit(key_t key, int value);
void semWait(int sid);
void semSignal(int sid);

/* Message Queue */
struct content {
    char sockaddr[16];
    unsigned port;
    int connfd;
};

struct message {
    long mtype;
    struct content mcontent;
};

int Msgget(key_t key, int msgflag);
int Msgsnd(int msgqid, const void* msgptr, size_t msgsize, int msgflag);
ssize_t Msgrcv(int msgqid, void* msgptr, size_t msgsize, long msgtype, int msgflag);
int Msgctl(int msgqid, int cmd, struct msqid_ds* buf);

/* Share Memory Segment */
int Shmget(key_t key, size_t size, int shmflag);
void* Shmat(int shmid, const void* shmaddr, int shmflag);

#endif