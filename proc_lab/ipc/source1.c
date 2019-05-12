main()
{
    int i, pid, status = 1;
    while ((i = fork()) == -1)
        ;
    if (i) { /* 父进程 */
        printf("It is the parent process.\n");
        pid = wait(&status);
        printf("Child process %d,status =%d \n", pid, status);
    } else { /* 子进程 */
        printf("It is the child process.\n");
        execl("/bin/ls", "ls", "-l", (char*)0); /* 映像改换 */
        printf("execl  error.\n"); /* 映像改换失败 */
        exit(2);
    }
    printf("Parent process finish. \n");
}
