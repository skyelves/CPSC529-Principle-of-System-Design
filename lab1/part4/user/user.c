#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "mysyscall.h"

#define TEST_THREAD_NUM 5

pid_t pid;

void test_syscall(pid_t _pid, int _type){
    long res = mysyscall(440, _pid, _type);
    if (_type == 1){
        printf("user stack memory cost: %ld\n", res);
    } else if (_type == 2){
        printf("kernel stack memory cost: %ld\n", res);
    } else if (_type == 3){
        printf("heap memory cost: %ld\n", res);
    }
}

void *kernel_stack_test_func(void* args) {
	int i = 1;
    sleep(1);
}

void user_stack_test_func(int depth){
    if (depth == 512){
        test_syscall(pid, 1);
        return;
    }
    user_stack_test_func(depth + 1);
}

int main()
{
    pid = getpid();
    int memory_type[3] = {1, 2, 3};
    for (int i = 0; i < 3; i++){
        test_syscall(pid, memory_type[i]);
    }

    // test heap
    char *tmp0 = (char *)malloc(16);
    test_syscall(pid, 3);
    char *tmp1 = (char *)malloc(4096);
    test_syscall(pid, 3);

    // test kernel stack, each thread has a kernel stack
    pthread_t pt[TEST_THREAD_NUM];
    for (size_t i = 0; i < TEST_THREAD_NUM; i++){
        pthread_create(&pt[i], NULL, kernel_stack_test_func, NULL);
    }
    test_syscall(pid, 2);
    for (size_t i = 0; i < TEST_THREAD_NUM; i++){
        pthread_join(pt[i], NULL);
    }

    // test user stack
    user_stack_test_func(0);

    
    return 0;
}