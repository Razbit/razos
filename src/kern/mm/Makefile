CSOURCES = kmalloc.c paging.c heap.c detect.c
ASMSOURCES = 
SUBOBJS = 

OBJS = $(patsubst %.c,%.c.o,$(CSOURCES))
OBJS += $(patsubst %.s,%.s.o,$(ASMSOURCES))


all: $(OBJS) link 

link:
	ld $(LDFLAGS) -o mm.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	gcc $(CFLAGS) $< -o $@

clean:
	rm *.o
