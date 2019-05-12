
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