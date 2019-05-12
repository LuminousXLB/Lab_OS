#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct message_st {
    long mtype;
    char mtext[80];
};

int main()
{

    const char* filename = "xato-net-10-million-passwords-10.txt";
    key_t MSGKEY = ftok(__FILE__, 5213);

    if (fork() > 0) {
        /* parent */
        pid_t pid = getpid();

        /* get the message queue */
        int mqid = msgget(MSGKEY, IPC_CREAT | 0666);
        if (mqid < 0) {
            perror("Parent msgget");
            exit(EXIT_FAILURE);
        }

        /* open datafile */
        FILE* pFile = fopen(filename, "r");
        if (pFile == NULL) {
            perror("Error opening file");
        }

        /* read file and send message with type 2 as valid data */
        struct message_st msg;
        msg.mtype = 2;
        srand(time(NULL));
        while (fgets(msg.mtext, 80, pFile) != NULL) {
            sleep(rand() % 2);
            msgsnd(mqid, &msg, sizeof(msg.mtext), 0);
            printf("%4d: Parent sent message %ld: %s", pid, msg.mtype, msg.mtext);
        }
        printf("%4d: Parent finished\n", pid);

        /* send message with type 3 as terminate notice */
        msg.mtype = 3;
        msgsnd(mqid, &msg, sizeof(msg.mtext), 0);

        fclose(pFile);
    } else {
        /* child */
        pid_t pid = getpid();

        /* get the message queue */
        int mqid = msgget(MSGKEY, IPC_CREAT | 0666);
        if (mqid < 0) {
            perror("Child msgget");
            exit(EXIT_FAILURE);
        }

        /* receive message and print to stdout */
        struct message_st msg;
        while (1) {
            msgrcv(mqid, &msg, 80, 0, MSG_NOERROR);
            if (msg.mtype == 2) {
                printf("%4d: Child received message %ld: %s", pid, msg.mtype, msg.mtext);
            } else if (msg.mtype == 3) {
                break;
            } else {
                exit(EXIT_FAILURE);
            }
        }

        /* finish and exit */
        printf("%4d: Child finished\n", pid);
        exit(0);
    }

    /* wait child to exit */
    while (waitpid(-1, 0, 0) > 0)
        ;
}
