// 父进程一次读入一个字符串,将其存放在共享内存中,子进程从共享内存中取出数据,输出打印｡
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SIZE 1024 /* 共享内存长度 */

union semun {
    int val; /* Value for SETVAL */
    struct semid_ds* buf; /* Buffer for IPC_STAT, IPC_SET */
    unsigned short* array; /* Array for GETALL, SETALL */
};

int creatsem(key_t key)
{
    printf("%4d: %s\n", __LINE__, __FUNCTION__);
    int sid;

    if ((sid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
        perror("semget");
    }

    union semun arg;
    arg.val = 1;

    if (semctl(sid, 0, SETVAL, arg) == -1) {
        perror("semctl");
    }
    return sid;
}

static void semcall(int sid, int op)
{
    printf("%4d: %s\n", __LINE__, __FUNCTION__);
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = op;
    sb.sem_flg = 0;
    if (semop(sid, &sb, 1) == -1) {
        perror("semop");
    }
};

void P(int sid)
{
    printf("%4d: %s\n", __LINE__, __FUNCTION__);
    semcall(sid, -1);
}

void V(int sid)
{
    printf("%4d: %s\n", __LINE__, __FUNCTION__);
    semcall(sid, 1);
}

int main()
{
    key_t SEMKEY1 = ftok(__FILE__, 19001);
    key_t SEMKEY2 = ftok(__FILE__, 19002);
    key_t SHMKEY = ftok(__FILE__, 18001);

    /* 创建两个信号灯,初值为1 */
    int sid1 = creatsem(SEMKEY1);
    int sid2 = creatsem(SEMKEY2);

    printf("%4d: %s\n", __LINE__, __FUNCTION__);

    char* segaddr;
    int segid;
    /* 创建共享内存段 */
    if ((segid = shmget(SHMKEY, SIZE, IPC_CREAT | 0666)) == -1) {
        perror("shmget");
    }
    /* 将共享内存映射到进程数据空间 */
    segaddr = shmat(segid, 0, 0);

    printf("%4d: %s\n", __LINE__, __FUNCTION__);

    P(sid2); /* 置信号灯2值为0,表示缓冲区空 */
    if (fork() == 0) {
        /* 子进程,接收和输出 */
        while (1) {
            P(sid2);
            printf("Received from Parent: %s\n", segaddr);
            V(sid1);
        }
    } else {
        /* 父进程,输入和存储 */
        while (1) {
            P(sid1);
            scanf("%s", segaddr);
            V(sid2);
        }
    }
}
