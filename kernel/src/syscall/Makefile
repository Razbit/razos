CSOURCES = syscall.c sys_exit.c sys_yield.c sys_fork.c sys_wait.c sys_regdump.c sys_console_log.c
ASMSOURCES = syscall.s
SUBOBJS = 
OBJS = $(patsubst %.c,%.c.o,$(CSOURCES))
OBJS += $(patsubst %.s,%.s.o,$(ASMSOURCES))


all: $(OBJS) link 

link:
	$(LD) $(LDFLAGS) -o syscall.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
