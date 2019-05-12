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
