# Project 4: Kernel Hacking

In this project, I implemented the kernel hacking project with the following functionality: a system call `getreadcount` and a user program called `readcount.c` that prints the amount of times the system call "read" has been called, and a possibility to clear the counter. No functionality was made to count for different system calls, so ideally this project would be worth **4 points**.

# Usage

To start with my project, cd into the folder `xv6-riscv` and run the following commands to compile the kernel and run it with qemu. Look into the original github page for needed dependencies etc https://github.com/mit-pdos/xv6-riscv./  
```
make clean
make
make qemu
```
Note that I had to change line 247 in the file `./xv6-riscv/user/usertests.c` to make the compile not fail on my computer, so if that causes problems just change the line to `rwsbrk()`.  

After this, you can test the new system call with the user program by calling `readcount` in the shell. By adding the right argument `readcount clear`, the counter will be reset to 0. You can test it with other user programs that use system calls such as `ls`, but calling the readcount itself will use some system calls.  

# Documentation and changes made

The changes made include all the additions to the kernel and the user side, and also the user program. I will first list out the changes inside the kernel (`./xv6-riscv/kernel`):  

In the file `sysfile.c`, I created a variable to keep track of the times read has been called, and I increment it every time "read" is called. The actual system call is implemented in `sysproc.c`, since in `sysfile.c` there are file related system calls, which this is not.  

In `syscall.c` and `syscall.h` I just added a few lines to define the getreadcount prototype and add it to the array of syscalls. This was mostly just done by looking at the code and how the other system calls were defined.  

In the userside `./xv6-riscv/user`, following changes were made:  

The user program `readcount.c` was made, which very simply just calls the newly defined system call in the kernel, assigns the return value to a variable and prints it out. It also checks if the additional argument "clear" is given, and if it is, assigns it to value 1 which will trigger the reset for the counter.  

In `usys.pl` I create a stub entry for the new system call, as is done with all the other system calls. The system call is also added to the `user.h`.  

Finally, in the root folder of the modified xc6-riscv, I add the user program to the Makefile, which concludes the changes made in this project.  

# Screenshots of running program
This screenshot showcases all the functions in my customized kernel, utilizing the user program I made. Note that readcount itself increases the counter, most likely because using the shell itself calls the read system call, incrementing it. I also show that the argument "clear" needs to be correct for the counter to be cleared\
![Alt text](./testgetreadcount.png?raw=true "Optional Title")

# AI usage.

AI was used to ask about the xv6 kernel, but no code was generated.