#ifndef __W_SEMAPHORE_H__
#define __W_SEMAPHORE_H__

#include <stdbool.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

/* Semaphore */
union semun {
    int val; /* Value for SETVAL */
    struct semid_ds* buf; /* Buffer for IPC_STAT, IPC_SET */
    unsigned short* array; /* Array for GETALL, SETALL */
};

int semInit(key_t key, int value);
void semWait(int sid);
void semSignal(int sid);
#endif