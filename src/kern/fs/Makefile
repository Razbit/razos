CSOURCES = fs.c initrd.c
ASMSOURCES = 
SUBOBJS = 

OBJS = $(patsubst %.c,%.c.o,$(CSOURCES))
OBJS += $(patsubst %.s,%.s.o,$(ASMSOURCES))


all: $(OBJS) link 

link:
	ld $(LDFLAGS) -o fs.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	gcc $(CFLAGS) $< -o $@

clean:
	rm *.o
