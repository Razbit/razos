CSOURCES = syscall.c sys_tasking.c sys_fs.c sys_setup.c sys_pipe.c sys_uvm.c sys_execve.c sys_time.c
ASMSOURCES = syscall.s
SUBOBJS = 
OBJS = $(patsubst %.c,%.c.o,$(CSOURCES))
OBJS += $(patsubst %.s,%.s.o,$(ASMSOURCES))


all: $(OBJS) link 

link:
	$(LD) $(LDFLAGS) -o syscall.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
