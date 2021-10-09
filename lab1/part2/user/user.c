#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
int main()
{
    pid_t pid[10] = {0, 1, 10, 132, 1334, 111111};
    int memory_type[10] = {0, 1, 2, 2, 1, 2, 0};
    for (size_t i = 0; i < 6; i++)
    {
        long int res = syscall(440, pid[i], memory_type[i]);
        printf("System call memuse returned %ld\n", res);
    }
    return 0;
}