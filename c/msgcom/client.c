#include "msgcom.h"

main() {
  struct msgtype buf;
  int qid, pid;
  qid = msgget(MSGKEY, IPC_CREAT | 0666);
  buf.mtype = 1;
  buf.text = pid = getpid();
  msgsnd(qid, &buf, sizeof(buf.text), 0);
  msgrcv(qid, &buf, 512, pid, MSG_NOERROR);
  printf("Request received a massags from server, type is: % d\n", buf.mtype);
}
