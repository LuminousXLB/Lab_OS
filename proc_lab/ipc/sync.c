#include "sync.h"

void initAllocator(struct semProducerConsumer* spc)
{
    key_t SEMKEY1 = ftok(__FILE__, 19001);
    key_t SEMKEY2 = ftok(__FILE__, 19002);
    key_t SHMKEY = ftok(__FILE__, 18001);

    int segid;
    /* 创建共享内存段 */
    if ((segid = shmget(SHMKEY, sizeof(bool) * SERVANT_COUNT, IPC_CREAT | 0666)) == -1) {
        unix_error("shmget");
    }

    /* 创建两个信号灯,初值为1 */
    spc->available = semInit(SEMKEY1, 4);
    spc->mutex = semInit(SEMKEY2, 1);
    spc->shmid = segid;
    spc->status = (int*)shmat(segid, 0, 0);
    int i;
    for (i = 0; i < SERVANT_COUNT; i++) {
        spc->status[i] = 1;
    }
}

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

/* Semaphore */
int semInit(key_t key, int value)
{
    int sid;

    if ((sid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
        unix_error("semget");
    }

    union semun arg;
    arg.val = value;

    if (semctl(sid, 0, SETVAL, arg) == -1) {
        unix_error("semctl");
    }
    return sid;
}

static void _semcall(int sid, int op)
{
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = op;
    sb.sem_flg = 0;
    if (semop(sid, &sb, 1) == -1) {
        unix_error("semop");
    }
};

void semWait(int sid)
{
    _semcall(sid, -1);
}

void semSignal(int sid)
{
    _semcall(sid, 1);
}

/* Message Queue */
int Msgget(key_t key, int msgflag)
{
    int qid = msgget(key, msgflag);
    if (qid < 0) {
        unix_error("msgget");
    }
    return qid;
}

int Msgsnd(int msgqid, const void* msgptr, size_t msgsize, int msgflag)
{
    int ret = msgsnd(msgqid, msgptr, msgsize, msgflag);
    if (ret < 0) {
        unix_error("msgsnd");
    }
    return ret;
}

ssize_t Msgrcv(int msgqid, void* msgptr, size_t msgsize, long msgtype, int msgflag)
{
    ssize_t len = msgrcv(msgqid, msgptr, msgsize, msgtype, msgflag);
    if (len < 0) {
        unix_error("msgrcv");
    }
    return len;
}

int Msgctl(int msgqid, int cmd, struct msqid_ds* buf)
{
    int ret = msgctl(msgqid, cmd, buf);
    if (ret < 0) {
        unix_error("msgsnd");
    }
    return ret;
}

/* Share Memory Segment */
int Shmget(key_t key, size_t size, int shmflag)
{
    int shmid = shmget(key, size, shmflag);
    if (shmid < 0) {
        unix_error("shmget");
    }
    return shmid;
}

void* Shmat(int shmid, const void* shmaddr, int shmflag)
{
    void* shm_nattch = shmat(shmid, shmaddr, shmflag);
    if ((int)shm_nattch == -1) {
        unix_error("shmat");
    }
    return shm_nattch;
}
