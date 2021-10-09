#include <linux/kernel.h>   
#include <linux/syscalls.h>
#include <linux/pid.h> 

SYSCALL_DEFINE2(memuse_syscall, pid_t, pid, int, memory_type){
        rcu_read_lock();
        struct pid* res = find_vpid(pid);
        rcu_read_unlock();
        printk("pid:%d, memory_type:%d\n", pid, memory_type);
        return (res == NULL ? -1 : 1);
}