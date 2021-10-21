#include <linux/kernel.h>   
#include <linux/syscalls.h>
#include <linux/pid.h> 

SYSCALL_DEFINE2(memuse_syscall, pid_t, pid, int, memory_type){
        // acquite the rcu lock
        rcu_read_lock();
        //check if the process exists
        struct pid* res = find_vpid(pid);
        // print the info
        printk("pid:%d, memory_type:%d\n", pid, memory_type);
        // release the rcu lock
        rcu_read_unlock();
        // check the memory type
        if((memory_type > 3) || (memory_type < 1))
                return -1;
        return (res == NULL ? -1 : 1);
}