Writing syscalls to RazOS
=========================

Writing syscalls is a rather simple thing in the RazOS kernel.
Remember to take a look into the /kernel/src/syscall and /rlibc directories!
Here's a quick how-to:

##Writing the syscall
###In the kernel
- Include ```<api/razos.h>``` and ```"syscall.h"``` to your files
- Use the ARG -macros from ```syscall.h``` to fetch arguments on the kernel side

###In user-space
- Include ```<api/razos.h>``` to your wrapper
- Use the ```__syscall``` -functions from ```api/razos.h``` (first parameter is *always* the syscall number) for calling the kernel-side of your syscall
- Arguments should be cast to ```uint32_t```

##Add to the kernel
1. Add your syscall source files (```sys_your_syscall.c```, ```sys_your_syscall.h```)
in /kernel/src/syscall/
2. Add your syscall to the ```razos_kernel_headers/api/razos.h``` (```SYS_YOUR_SYSCALL NUM```)
3. Include your syscall's header to ```syscall.c```
4. Add your syscall to the ```syscall_table``` in ```syscall.c```
5. Add the .c files to the appropriate Makefiles
