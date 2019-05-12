#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SIZE 1024 /* 共享内存长度 */
#define true 1
#define false 0

union semun {
    int val; /* Value for SETVAL */
    struct semid_ds* buf; /* Buffer for IPC_STAT, IPC_SET */
    unsigned short* array; /* Array for GETALL, SETALL */
};

int creatsem(key_t key, int init)
{
    int sid;
    if ((sid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
        perror("semget");
    }

    union semun arg;
    arg.val = init;

    if (semctl(sid, 0, SETVAL, arg) == -1) {
        perror("semctl");
    }
    return sid;
}

static void semcall(int sid, int op)
{
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = op;
    sb.sem_flg = 0;
    if (semop(sid, &sb, 1) == -1) {
        perror("semop");
    }
};

void semWait(int sid)
{
    semcall(sid, -1);
}

void semSignal(int sid)
{
    semcall(sid, 1);
}

#define SERVANT_COUNT 4
struct semProducerConsumer {
    int available;
    int mutex;
    int shmid;
    int* status;
};

int allocServant(struct semProducerConsumer* spc)
{
    semWait(spc->available);
    semWait(spc->mutex);
    int i;
    for (i = 0; i < SERVANT_COUNT; i++) {
        if (spc->status[i]) {
            break;
        }
    }
    spc->status[i] = false;
    semSignal(spc->mutex);
    return i;
}

void freeServant(struct semProducerConsumer* spc, int i)
{
    semWait(spc->mutex);
    semSignal(spc->mutex);
    spc->status[i] = true;
    semSignal(spc->available);
}

void init(struct semProducerConsumer* spc)
{

    key_t SEMKEY1 = ftok(__FILE__, 19001);
    key_t SEMKEY2 = ftok(__FILE__, 19002);
    key_t SHMKEY = ftok(__FILE__, 18001);

    int segid;
    /* 创建共享内存段 */
    if ((segid = shmget(SHMKEY, SIZE, IPC_CREAT | 0666)) == -1) {
        perror("shmget");
    }

    /* 创建两个信号灯,初值为1 */
    spc->available = creatsem(SEMKEY1, 4);
    spc->mutex = creatsem(SEMKEY2, 1);
    spc->shmid = segid;
    spc->status = (int*)shmat(segid, 0, 0);
    int i;
    for (i = 0; i < SERVANT_COUNT; i++) {
        spc->status[i] = 1;
    }
}

int main()
{
    struct semProducerConsumer spc_status;
    init(&spc_status);
    srand(time(NULL));

    // semWait(sid2); /* 置信号灯2值为0,表示缓冲区空 */

    pid_t cpid;
    int i;
    for (i = 0; i < SERVANT_COUNT; i++) {
        sleep(rand() % 5);
        if ((cpid = fork()) == 0) {
            srand(time(NULL));
            pid_t pid = getpid();
            int sec = rand() % 20;

            /* 子进程,接收和输出 */
            int servant_id = allocServant(&spc_status);
            printf("%d | servant %d: got, sleep %d seconds. \t", pid, servant_id, sec);
            for (i = 0; i < SERVANT_COUNT; i++) {
                printf("%d ", spc_status.status[i]);
            }
            printf("\n");

            sleep(sec);

            freeServant(&spc_status, servant_id);
            printf("%d | servant %d: released. \t", pid, servant_id);
            for (i = 0; i < SERVANT_COUNT; i++) {
                printf("%d ", spc_status.status[i]);
            }
            printf("\n");

            sleep(rand() % 10);

            servant_id = allocServant(&spc_status);
            printf("%d | servant %d: got, sleep %d seconds. \t", pid, servant_id, sec);
            for (i = 0; i < SERVANT_COUNT; i++) {
                printf("%d ", spc_status.status[i]);
            }
            printf("\n");

            sleep(rand() % 20);

            freeServant(&spc_status, servant_id);
            printf("%d | servant %d: released. \t", pid, servant_id);
            for (i = 0; i < SERVANT_COUNT; i++) {
                printf("%d ", spc_status.status[i]);
            }
            printf("\n");

            exit(0);
        }
    }

    while (waitpid(-1, 0, 0) > 0)
        ;
}
