CSOURCES = vfs.c ramfs.c initrd.c stdout.c stderr.c stdin.c stat.c fifofs.c pipe.c do_fcntl.c
ASMSOURCES = 
SUBOBJS = 

OBJS = $(patsubst %.c,%.c.o,$(CSOURCES))
OBJS += $(patsubst %.s,%.s.o,$(ASMSOURCES))


all: $(OBJS) link 

link:
	$(LD) $(LDFLAGS) -o fs.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
