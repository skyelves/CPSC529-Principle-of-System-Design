# Lab2 Report

## Part1

The docs is easy to read and elaborates Rust detailedly. With the toy example in the docs, we can easily get into use of Rust. 

When I am reading the docs, the most impressing thing is the safety which Rust guarantees. In C/C++, even the most proficient programer can make some seemingly stupid problems, such as overflow and out-of-bounds accesses. However, in Rust, it prevents these problems in the compiling time and reduces the possibility of error in the runtime. Also Rust enforces the programers to implicitly consider the memory allocations, objects copy and lifetime in an elegant way. With more efforts in the compiling time, the runtime can be much safer and the overhead can be much cheaper.

Another thing, which is also impressing, is Cargo, Rust’s build system and package manager. Unlike C/C++ programs, we always suffer from compiling error due to the different APIs in different versions of dependencies. We can never build and install a massive open-source project at the first try. However, with the dependencies specified, it's much easier to transport the program from one platform to another platform or from one server to another server.

In a nutshell, It's very interesting to learn Rust and it's certainly promising to build network stack, database or even OS in Rust.

