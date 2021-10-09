#include <linux/kernel.h>   
#include <linux/syscalls.h>
#include <linux/pid.h> 

// asmlinkage long memuse_syscall(unsigned int pid, unsigned int memory_type)
// {
//         struct pid* res = get_pid(find_vpid(nr));
//         printk("pid:%d, memory_type:%d\n", pid, memory_type);
//         return res?NULL:-1,0;
// }

SYSCALL_DEFINE2(memuse_syscall, pid_t, pid, int, memory_type){
        struct pid* res = get_pid(find_vpid(pid));
        printk("pid:%d, memory_type:%d\n", pid, memory_type);
        return (res==NULL?-1:1);
        // return 0;
}