CSOURCES = device.c devfs.c vfs.c initrd.c ramfs.c stat.c do_fcntl.c pipe.c stdio_vfs.c stdout.c stderr.c stdin.c fat/fat.c
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
	rm fat/*.o
