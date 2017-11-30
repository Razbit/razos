CSOURCES = exec.c load_elf.c
ASMSOURCES =
SUBOBJS = 

OBJS = $(patsubst %.c,%.c.o,$(CSOURCES))
OBJS += $(patsubst %.s,%.s.o,$(ASMSOURCES))


all: $(OBJS) link 

link:
	$(LD) $(LDFLAGS) -o loader.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
