#include <signal.h>
#include <sys/types.h>

main()
{
    int status;
    pid_t pid;
    void func();
    signal(SIGUSR1, func); /* 预置信号处理程序 */
    if (pid = fork()) {
        printf("Parent: will send signal.\n");
        kill(pid, SIGUSR1); /* 发送信号 */
        wait(&status); /* 等待子进程停止 */
        printf("status=%d: Parent finished:\n", status);
    } else {
        sleep(10); /* 等待接受信号 */
        printf("Child:signal is received.\n");
        exit(0);
    }
}
void func() { printf("It is signal processing function.\n"); }
