### This repo includes the source codes of the projects in CPSC529 Principle of System Design at Yale.

## CPSC429/529 Programming Assignment 1 Dipping into the Linux Kernel

#### Goal

The goal of this programming assignment is to familiarize you with Linux kernel-level development and how C, used not only by the kernel itself but also most applications in the early days of Linux, has impacted the design and development of the Linux kernel. We intend this experience to contrast with what you will have with Programming Assignment 2. 

#### Functions

Both this programming assignment and the next one ask you to develop the same capability for the system under question: a kernel module that allows a user program to query the physical memory usage by a process. That is, the kernel module supports a function call that returns the number of physical pages used by the process named by its process ID. 
In this assignment, you will write your own kernel module that implements the above function in the form of *system call*. Doing so will expose you to the difficulty of programming userspace and kernel space components at the same time, the special ways in which userspace programs must communicate with the kernel, and the danger of using unsafe traditional languages like C in these environments.
The Linux kernel has the ability to load separate pieces of software into its memory and execute them long after the initial boot time, these are referred to as *loadable kernel modules.* Part of your tasks in completing this assignment is to become self-acquainted with the myriad of kernel-related resources and tutorials out there on the web. In other words, learn to Google properly! (For example, Google “how to write kernel modules” and “how to debug kernel modules”) 
HINT: You will most likely want to develop and test your kernel modules on a virtual machine, such as QEMU. Messing with the kernel can be dangerous: a bug in your custom kernel could render the machine unbootable and difficult to recover. See [this article](https://lwn.net/Articles/660404/) for tips on kernel development from the Linux devs themselves. 

#### Deliverables, Lab Report, Demo, and Deadlines

You will submit Lab 1 in two stages:Parts 1-3 are due on October 12th, 2021. Part 4 is due on October 29th, 2021. This will include your final lab report. 
You must submit your working code, including all module source files, modified kernel files, makefiles, and userspace test programs, in a .zip archive to Canvas AND as a git repository hosted on GitHub, GitLab, etc. Also, please insert comments into your code identifying what you changed, or include all the changes in a separate section in your lab report. 
Each student will need to set up a meeting with the TA to demonstrate his/her working system. The TA may ask the student questions about his/her code to gauge their experience and knowledge gained in the area -- don’t worry, it won’t be a formal quiz or anything. Also, your kernel modules and userspace programs do not need to output data in any specific format, just whichever way you see fit. 
Your lab report should include a brief discussion for each Part in this lab that summarizes your coding solution, in particular your experience in learning about and implementing your solution. Focus on any difficulties you experienced and how you overcame them. This is especially important for partial credit on any parts you were unable to complete. In addition, your lab report should include answers to the questions or discussion points in each Part. **These are written in blue boldfaced font** so you can easily locate them. For parts that ask quantitative questions, e.g., performance comparisons, please include an appropriate graph that shows the relevant performance measurements, along with the raw data in table form. These tables and graphs may be referenced in your meeting with the TA, so be prepared to explain them.
**Grading:** This lab has multiple parts, each worth the following amount for a total of 20 points:

Part1 1

Part2 3

Part3 1

Part4 5

Lab Report 10

### Part 1: Hello World from a kernel module
The first step is to create and set up a very simple kernel module that does nothing but print “Hello world” to the kernel log. For this part, simply create a source file called hello.c that implements the bare minimum of functions to get a module up and running. There are dozens of great tutorials on this matter all over the internet. 
You will need two functions (they can be named anything you want): module_init(): called when your module is loaded. This should print “Hello world!” module_exit(): called when your module is unloaded. Should print “Goodbye world!”
To print to the kernel log, you will need to call printk() from your kernel module. Note that in the kernel programming environment, there is no standard C library like libc, so there is no such function as printf(). Thus, the output of your module will be printed to the kernel log, *not* to the standard output on the terminal.
The following commands are very useful to monitor the status of kernel modules: dmesg: shows the output of the kernel log.insmod: loads the specified kernel module (usually a .ko file) into the kernel and runs it.modprobe is another way to load modules that handles dependencies better.lsmod: shows which modules are currently loaded. 
You should also create a standard Makefile in the same directory as hello.c so that you can build your entire module just by running the command make. Also, Makefiles are standard practice and allow for much easier project setup. For example, you’ll need to specify the base directory of the kernel source, typically by setting a variable like KDIR (but it can be called anything you want). Again, Google is your friend. 

### Part 2: Adding a new system call

The Linux kernel exposes services to the user space via system calls, a well-defined and historically stable interface. In this part, you will create a brand new custom system call and add it to the Linux kernel. The system call must exist as a built-in module in the kernel, it cannot be implemented as a loadable module that is inserted after boot. 
*NOTE*: older Linux kernel versions (before v2.6) allowed you to add new system calls and overwrite/intercept existing system calls just by writing some special code in a kernel module. 
**Describe why you think the Linux kernel no longer allows loadable kernel modules to modify syscall tables or add system calls.** 
Note that you are still able to create a loadable module that exposes a function that can be called by your newly-added syscall. This can make development easier but is not required. If you choose this route, your built-in syscall code must first check that the module function is available before calling it. 
HINT: use function pointers to dynamically change which function is called. 
Your system call should be named “**memuse_syscall**” and should be assigned to the next biggest number in the syscall table. In other words, if your kernel’s last entry in the syscall table is syscall number 393, then yours will be number 394. 
*HINT*: syscall tables are defined in several areas across the kernel, but they must always be present in **unistd.h**. As with prior parts, Google is your friend --- there are hundreds of excellent Linux kernel syscall tutorials on the internet, but be sure to use an up-to-date one, because things in the kernel have changed over time. 
Your syscall, memuse_syscall, must do the following simple things::Accept two int arguments: the first indicates the process identifier PID and the second specifies the type of memory usage: 1 for call stack, 2 for kernel stack, and 3 for the heap. Print that input arguments to the kernel log.Return 1 if the process of the specified PID exists; return -1 if otherwise or an error happens.
Finally, create a simple test user space program that invokes your new system call using the C library’s **syscall()** method: http://man7.org/linux/man-pages/man2/syscall.2.html. All you have to do is pass the correct arguments.
**What happens if you call** **syscall()** **with a bogus syscall number that doesn’t exist? How could a userspace program prevent that error without relying on the kernel to catch it?**
**Describe which files you needed to change to add a new system call and how you changed them to accommodate your new syscall. Also describe which safety conditions you check within your kernel implementation of** **memuse_syscall****, and why each check is necessary. What, if any, extra arguments did you add to the syscall to help check whether the inputs coming from userspace are valid?**

### Part 3: Interfacing from user space to the kernel on x86

In the previous parts, it was really easy to invoke your system call from a userspace program! All you had to do was include the C library’s existing syscall() function in your program and then call it with the number of your newly-created syscall. 
But we still don’t understand exactly what happens when a syscall is invoked. There must be some way for the software to tell the hardware to switch from user space (non-privileged) mode to the privileged execution mode in kernel space. 
Typically, your C program will call a function in libc, which then translates your requested system call into a set of assembly instructions. In this part, you must implement your own syscall() invocation routine directly in your userspace program.
*You are not allowed to use any existing functions from libc or any other library to accomplish this; all code that invokes a syscall must be your own.*
Create a copy of your userspace program from Part 2 and replace that syscall() function with your own definition. 
**What is the limit on the number of parameters a system call can accept? Why is it that number, i.e., what is the limiting factor? How could you add more parameters if you needed to pass more arguments to a system call?**

### Part 4: Interfacing from user space to the kernel on x86
Now we get memuse_syscall to do real work:
Return the size of physical memory used in the specified type. Return -1 if the specified process does not exist or an error happens.
You are not allowed to use information available from [the /proc file system](https://www.kernel.org/doc/Documentation/filesystems/proc.txt). Many popular tools such as ps and pmap are actually based on /proc. The /proc file system is another interface between the user and kernel spaces in Linux-based systems.
To demonstrate part 4 you need to write a small userspace program. The program should be able to increase the size of its heap (e.g. using malloc) and call stack (e.g. using a recursive function). Then you should use the userspace program in part 3 to invoke your system call and measure the call stack, kernel stack and heap of the program you wrote for part 4.



## CPSC 429/529 Programming Assignment 2*A Taste of Rust and a weird OS* 

### Goal

The goal of this programming assignment is to show you how a safe language, specifically Rust, could allow software, specifically operating systems, to be developed with radically different architectures from those developed in C, like the Linux kernel. We intend this experience to contrast with that of Programming Assignment 1. 

### Functions

Both this programming assignment and the previous one ask you to develop the same capability for the system under question: a kernel module that allows a user program to query the physical memory usage by a process. That is, the kernel module supports a function call that returns the number of physical pages used by the process named by its process ID. (In Theseus, `process` is called `task`.)
In this assignment, you will write your own kernel module that implements the above function in the form of *function call*. Doing so will expose you to the ease of programming userspace and kernel space components at the same time, the boring ways in which userspace programs could communicate with the kernel, and the frustration of dealing with a bossy compiler like Rust’s.
Theseus OS has the ability to load separate pieces of software into its memory and execute them long after the initial boot time, these are referred to as *cells,* realized as Rust crates*.* To complete this assignment, you will need to become acquainted with Rust and Theseus OS. Online resources for Rust programming are abundant. However, that is not as true for Theseus OS. 

```HINT: You will most likely want to develop and test your Theseus cells on a virtual machine, such as QEMU. Messing with the OS can be dangerous: a bug in your custom OS could render the machine unbootable and difficult to recover. ```

### Deliverables, Lab Report, Demo, and Deadlines

You will submit Lab 2 according to the schedule summarized in the table below.You must submit your working code, including all module source files, modified kernel files, makefiles, and userspace test programs, in a .zip archive to Canvas AND as a git repository hosted on GitHub, GitLab, etc. Also, please insert comments into your code identifying what you changed, or include all the changes in a separate section in your lab report. Please share your repositories with ‘NamiLiy’ and ‘forrestzhong@gmail.com’ 
Each student will need to set up a meeting with the TA to demonstrate his/her working system. The TA may ask the student questions about his/her code to gauge their experience and knowledge gained in the area -- don’t worry, it won’t be a formal quiz or anything. Also, your kernel modules and userspace programs do not need to output data in any specific format, just whichever way you see fit. 
Your lab report should include a brief discussion for each part in this lab that summarizes your coding solution, in particular your experience in learning about and implementing your solution. Focus on any difficulties you experienced and how you overcame them. This is especially important for partial credit on any parts you were unable to complete. In addition, your lab report should include answers to the questions or discussion points in each part. **These are written in blue boldfaced font** so you can easily locate them. For parts that ask quantitative questions, e.g., performance comparisons, please include an appropriate graph that shows the relevant performance measurements, along with the raw data in table form. These tables and graphs may be referenced in your meeting with the TA, so be prepared to explain them.

#### Grading: 

This lab has multiple parts, each worth the following amount for a total of 20 points:
PartPointsDue DatePart 11November 9Part 22November 16Part 32November 18Part 45December 14Lab Report10

### Tutorial Materials
**Rust**

 [The Rust Language Book](https://doc.rust-lang.org/book/) (Complete reference).

 [Rust vs. C, with short Rust intro](https://docs.google.com/presentation/d/e/2PACX-1vQYomAnfTNucuCqYgNkPaxpIdrhPxil9Qzle_6-xd7TYfdEBlgML0B3vztdNC2odwc25dLzW3XsithZ/pub?start=false&loop=false) — [[Video Talk](https://www.youtube.com/watch?v=mmJiwscpB4o)] (a quick tour for C programmers)

**Theseus**

[Source code](https://github.com/theseus-os/Theseus)

[Book-style documentation](https://theseus-os.github.io/Theseus/book/index.html)


The two presentations by the primary developer of Theseus, Kevin Boos, will be very useful.

[How Safe-language OSes work, with Theseus examples](https://docs.google.com/presentation/d/e/2PACX-1vSa0gp8sbq8S9MB4V-FYjs6xJGIPm0fsZSVdtZ9U2bQWRX9gngwztXTIJiRwxtAosLWPk0v60abDMTU/pub?start=false&loop=false) — [[Video Talk](https://www.youtube.com/watch?v=n7r8zO7SodE)]


### Part 1: Start with Rust
Read Chapters 1 to 4 of [the Rust Language Book](https://doc.rust-lang.org/book/). Try out all the examples. Watch the following video:[Rust vs. C, with short Rust intro](https://docs.google.com/presentation/d/e/2PACX-1vQYomAnfTNucuCqYgNkPaxpIdrhPxil9Qzle_6-xd7TYfdEBlgML0B3vztdNC2odwc25dLzW3XsithZ/pub?start=false&loop=false) — [[Video Talk](https://www.youtube.com/watch?v=mmJiwscpB4o)] (a quick tour for C programmers)**Describe your experience in learning and using Rust.**

### Part 2: Hello, World! in Theseus

Please build and run Theseus OS and then write a “Hello, World!” app for Theseus. 
You should be able to compile the app, load its binary to Theseus, and run it from the terminal. The app should print out “Hello, World!” to the terminal and return.
You may want to watch the two presentations by the primary developer of Theseus, Dr. Kevin Boos.[How Safe-language OSes work, with Theseus examples](https://docs.google.com/presentation/d/e/2PACX-1vSa0gp8sbq8S9MB4V-FYjs6xJGIPm0fsZSVdtZ9U2bQWRX9gngwztXTIJiRwxtAosLWPk0v60abDMTU/pub?start=false&loop=false) — [[Video Talk](https://www.youtube.com/watch?v=n7r8zO7SodE)][A Programmer's Introduction to Theseus](https://docs.google.com/presentation/d/e/2PACX-1vQuDoQq0mKf2r4m3xMeZ4LVao2Ngh6HPHWCdJASW9uasaRSbWaRvHc2LoZD2bTpIOHUkKeN6VjP8KJG/pub?start=false&loop=false)
**Describe your experience and any interesting observations.** 

### Part 3: memuse interface

There is no “system call” in Theseus. A crate can export an interface by marking it as pub.
Please implement a Theseus kernel crate that exports a pub function named memuse. memuse must do the following simple things:Accept two arguments: The task identifier (task id) The type of memory usage: heap or call stack.Print that input argument to [the system log](https://theseus-os.github.io/Theseus/doc/logger/index.html).Return a result indicating whether the task described by task id exists or not. (Look into [the `Result` type](https://doc.rust-lang.org/rust-by-example/error/result.html)) 
Finally, create a simple app that invokes memuse. All you have to do is pass the correct arguments. The app should print out the results to the terminal.
**Describe your experience and any interesting observations. Contrast the system call interface in Linux with the interface in Theseus. Explain why Theseus can export OS functions like that. Why can Theseus run applications in the same privilege level as the kernel? Why doesn’t Theseus need a kernel stack?**

### Part 4: memuse implementation

Now we get memuse to do real work:
Return the size of physical memory used in the specified type. Return an Error if the specified task does not exist or an error happens.
Hint: In the Linux kernel, each process has its own virtual address space and therefore, its own heap. Theseus, however, uses a single virtual address space for all tasks and the OS. As a result, tasks in Theseus share system-wide per-core heaps. For tracking the size of the memory used for each task, you may need to modify the kernel crate used for memory allocation and deallocation. Theseus implements the heap interface defined by the [GlobalAlloc trait](https://doc.rust-lang.org/stable/std/alloc/trait.GlobalAlloc.html). You may want to start with where this trait is implemented in Theseus.
To demonstrate Part 4 you need to write another app. This app should be able to increase the size of the heap (e.g. using [the alloc crate](https://doc.rust-lang.org/alloc/)) and call stack (e.g. using a recursive function). Then you should use the app you wrote in Part 3 to invoke memuse and measure the call stack, and the size of the heap for the app you wrote for Part 4.
Tip : You can spawn another terminal and have them side by side. Use Ctrl + Alt + T to spawn a terminal, and then Super + Left Arrow or Right Arrow to move it to the left or right half of the screen.
**Describe your experience and any interesting observations. Explain the pros and cons of per-process virtual address space (Linux) vs. one virtual address space for all (Theseus). Explain why Theseus could safely use a single address space for all. Explain the benefits of having tasks sharing the same heaps. Explain why Theseus implements one heap for core.** 