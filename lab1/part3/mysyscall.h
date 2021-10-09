#include <stdarg.h>
#include <errno.h>

long int mysyscall(int number, int arg1, int arg2){	
    long resultvar=0;	
    asm volatile (
        "movl %1, %%eax\n\t"		
        "movl %2, %%edi\n\t"	
        "movl %3, %%esi\n\t"		
        "syscall\n\t"							
        : "=a" (resultvar)							
        : "r" (number), "r" (arg1), "r" (arg2)	
    );			
    return (long int) resultvar;
}

// long int mysyscall (long int __ssyscallysno, ...)
// {			       
//   /* FIXME: Keep this matching INLINE_SYSCALL for hppa */
//   va_list args;
//   long int arg0, arg1, arg2, arg3, arg4, arg5;
//   long int __sys_res;
//   /* Load varargs */
//   va_start (args, __sysno);
//   arg0 = va_arg (args, long int);
//   arg1 = va_arg (args, long int);
//   arg2 = va_arg (args, long int);
//   arg3 = va_arg (args, long int);
//   arg4 = va_arg (args, long int);
//   arg5 = va_arg (args, long int);
//   va_end (args);
//   {
//     LOAD_ARGS_6 (arg0, arg1, arg2, arg3, arg4, arg5)
//     register unsigned long int __res asm("r28");
//     PIC_REG_DEF
//     LOAD_REGS_6
//     asm volatile (SAVE_ASM_PIC
//                   "        ble  0x100(%%sr2, %%r0)        \n"
//                   "        copy %1, %%r20                \n"
//                   LOAD_ASM_PIC
//                   : "=r" (__res)
//                   : "r" (__sysno) PIC_REG_USE ASM_ARGS_6
//                   : "memory", CALL_CLOB_REGS CLOB_ARGS_6);
//     __sys_res = __res;
//   }
//   if ((unsigned long int) __sys_res >= (unsigned long int) -4095)
//     {
//       __set_errno (-__sys_res);
//       __sys_res = -1;
//     }
//   return __sys_res;
// }