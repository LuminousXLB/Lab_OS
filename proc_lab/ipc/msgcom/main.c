#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MSGKEY 5678

struct msgtype {
    long mtype;
    int text;
};

void sigchld_hdlr(int signo)
{
    int status, pid;
    while ((pid = waitpid(-1, &status, 0)) > 0) {
        if (WIFEXITED(status)) {
            printf("%d exit status %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("%d terminate sinal %d\n", pid, WTERMSIG(status));
        } else if (WIFSTOPPED(status)) {
            printf("%d stop sinal %d\n", pid, WSTOPSIG(status));
        } else if (WIFCONTINUED(status)) {
            printf("%d continue\n", pid);
        }
    }
}

int main()
{
    signal(SIGCHLD, sigchld_hdlr);
    perror("signal");

    if (fork() > 0) {
        // parent
        int qid = msgget(MSGKEY, IPC_CREAT | 0666);
        if (qid < 0) {
            perror("Parent msgget");
            exit(EXIT_FAILURE);
        }

        sleep(5);
        int pid = getpid();

        struct msgtype buf;
        buf.mtype = 1;
        buf.text = pid;

        msgsnd(qid, &buf, sizeof(buf.text), 0);
        msgrcv(qid, &buf, 512, pid, MSG_NOERROR);

        printf("Parent received a massags from server, type is: %ld\n", buf.mtype);
    } else {
        // child
        int qid = msgget(MSGKEY, IPC_CREAT | 0666);
        if (qid < 0) {
            perror("Child msgget");
            exit(EXIT_FAILURE);
        }

        struct msgtype buf;
        msgrcv(qid, &buf, 512, 1, MSG_NOERROR);
        printf("Child receive a request from process %d\n", buf.text);

        buf.mtype = buf.text;
        msgsnd(qid, &buf, sizeof(int), 0);
        exit(0);
    }
}
