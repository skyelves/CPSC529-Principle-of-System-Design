#include <linux/kernel.h>   
#include <linux/syscalls.h>
#include <linux/pid.h> 
#include <linux/mm_types.h>
#include <linux/sched.h>
#include <linux/types.h>

struct task_struct;
struct mm_struct;
struct vm_area_struct;

bool va_to_pa(struct mm_struct *mm, unsigned long va){
        pgd_t *pgd = pgd_offset(mm, va);
        if(pgd_present(*pgd)){
                p4d_t *p4d = p4d_offset(pgd, va);
                if(p4d_present(*p4d)){
                        pud_t *pud = pud_offset(p4d, va);
                        if (pud_present(*pud)){
                                pmd_t *pmd = pmd_offset(pud, va);
                                if(pmd_present(*pmd)){
                                        pte_t *pte = pte_offset_kernel(pmd, va);
                                        if(pte_present(*pte)){
                                                return 1;
                                        }
                                }
                        }
                }
        }
        return 0;
}

SYSCALL_DEFINE2(memuse_syscall, pid_t, pid, int, memory_type){
        unsigned long heap_size = 0, stack_size = 0, kernel_stack_size = 0;
        unsigned long i = 0, res = -1;
        printk("pid:%d, memory_type:%d\n", pid, memory_type);

        // no need for lock cause it already has a rcu read lock inside
        struct task_struct *ts = find_get_task_by_vpid(pid);
        if(ts == NULL){
                printk("Process %d doesn't exist!\n", pid);
                return -1;
        }

        struct mm_struct *base_mm = ts->mm;
        if(base_mm == NULL){ // It's a kernel process.
                printk("Process %d is a kernel process!\n", pid);
                return -1;
        }

        if(memory_type == 1){
                // user stack memory cost
                mmap_read_lock(base_mm);
                unsigned long start_process_stack = base_mm->start_stack;
                struct vm_area_struct *stack_vma = find_vma(base_mm, start_process_stack);
                unsigned long stack_start = stack_vma->vm_start, stack_end = stack_vma->vm_end;
                for (i = stack_start; i <= stack_end; i += PAGE_SIZE){
                        if(va_to_pa(base_mm, i)){
                                stack_size += PAGE_SIZE;
                        }
                }
                mmap_read_unlock(base_mm);
                printk("stack start addr: %lu, stack end addr: %lu\n", stack_start, stack_end);
                printk("stack size: %lu\n", stack_size);
                res = stack_size;
        } else if(memory_type == 2){
                // kernel stack memory cost
                struct task_struct *tmp = ts;
                unsigned long thread_num = 0;
                do{
                        void *tmp_kernel_stack = tmp->stack;
                        for (i = tmp_kernel_stack; i <= tmp_kernel_stack+THREAD_SIZE; i += PAGE_SIZE){
                                if(kern_addr_valid(i)){
                                        kernel_stack_size += PAGE_SIZE;
                                }
                        }
                        rcu_read_lock();
                        put_task_struct(tmp); // release the lock of the last task struct
                        tmp = next_thread(tmp);
                        get_task_struct(tmp); // get the lock of the current task struct
                        rcu_read_unlock();
                        thread_num++;
                } while (tmp != ts); // traverse the rcu loop
                printk("kernel stack size: %lu, thread number: %lu\n", kernel_stack_size, thread_num);
                res = kernel_stack_size;
        } else if(memory_type == 3){
                // user heap memory cost
                mmap_read_lock(base_mm);
                unsigned long start_process_heap = base_mm->start_brk;
                unsigned long end_process_heap = base_mm->brk;
                struct vm_area_struct *heap_vma = find_vma(base_mm, start_process_heap);
                unsigned long heap_start = heap_vma->vm_start, heap_end = heap_vma->vm_end;
                for (i = heap_start; i <= heap_end; i += PAGE_SIZE){
                        if(va_to_pa(base_mm, i)){
                                heap_size += PAGE_SIZE;
                        }
                }
                mmap_read_unlock(base_mm);
                printk("heap start addr: %lu, heap end addr: %lu\n", heap_start, heap_end);
                printk("heap size: %lu\n", heap_size);
                res = heap_size;
        }
        put_task_struct(ts);
        return res;
}