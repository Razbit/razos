CSOURCES = paging.c kvm.c uvm.c kmalloc.c task.c pagefault.c
ASMSOURCES = task.s
SUBOBJS = 

OBJS = $(patsubst %.c,%.c.o,$(CSOURCES))
OBJS += $(patsubst %.s,%.s.o,$(ASMSOURCES))


all: $(OBJS) link 

link:
	$(LD) $(LDFLAGS) -o mm.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
