#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "w_semaphore.h"

#define RESOURCE_COUNT 4

struct PCStatus {
    int available;
    int mutex;
    int shmid;
    bool* status;
};

void initAllocator(struct PCStatus* spc);
int allocResource(struct PCStatus* spc);
void freeResource(struct PCStatus* spc, int i);

int main()
{
    srand(time(NULL));

    struct PCStatus spc_status;
    initAllocator(&spc_status);

    int i;
    for (i = 0; i < 8; i++) {
        sleep(rand() % 5);
        if (fork() == 0) {
            /* Child Process */
            srand(time(NULL));

            pid_t pid = getpid();
            int times = rand() % 4;

            for (times; times >= 0; times--) {
                int sec = rand() % 20;
                int servant_id = allocResource(&spc_status);
                printf("%d | servant %d: got, sleep %d seconds. \n", pid, servant_id, sec);

                sleep(sec);

                freeResource(&spc_status, servant_id);
                printf("%d | servant %d: released. \n", pid, servant_id);
            }

            exit(0);
        }
    }

    while (waitpid(-1, 0, 0) > 0)
        ;
}

void initAllocator(struct PCStatus* spc)
{
    key_t SEMKEY1 = ftok(__FILE__, 19001);
    key_t SEMKEY2 = ftok(__FILE__, 19002);
    key_t SHMKEY = ftok(__FILE__, 18001);

    /* Create shared memory segmenation */
    int shmid = shmget(SHMKEY, sizeof(bool) * RESOURCE_COUNT, IPC_CREAT | 0666);
    if (shmid < 0) {
        unix_error("shmget");
    }

    /* 创建两个信号灯,初值为1 */
    spc->available = semInit(SEMKEY1, 4);
    spc->mutex = semInit(SEMKEY2, 1);
    spc->shmid = shmid;
    spc->status = (bool*)shmat(shmid, 0, 0);
    int i;
    for (i = 0; i < RESOURCE_COUNT; i++) {
        spc->status[i] = 1;
    }
}

int allocResource(struct PCStatus* spc)
{
    semWait(spc->available);
    semWait(spc->mutex);
    int i;
    for (i = 0; i < RESOURCE_COUNT; i++) {
        if (spc->status[i]) {
            break;
        }
    }
    spc->status[i] = false;
    semSignal(spc->mutex);
    return i;
}

void freeResource(struct PCStatus* spc, int i)
{
    semWait(spc->mutex);
    semSignal(spc->mutex);
    spc->status[i] = true;
    semSignal(spc->available);
}