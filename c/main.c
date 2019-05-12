#include <stdio.h>
#include <sys/msg.h>

int main(int argc, char const* argv[])
{
    printf("%d\n", MSGMNI);
    return 0;
}