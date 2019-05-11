#include "msgcom.h"

main() {
  struct msgtype buf;
  int qid;
  if ((qid = msgget(MSGKEY, IPC_CREAT | 0666)) == -1)
    return (-1);
  while (1) {
    msgrcv(qid, &buf, 512, 1, MSG_NOERROR);
    printf("Server receive a request from process % d\n", buf.text);
    buf.mtype = buf.text;
    msgsnd(qid, &buf, sizeof(int), 0);
  }
}
