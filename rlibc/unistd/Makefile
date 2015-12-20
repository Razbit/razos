CSOURCES = close.c lseek.c read.c write.c
ASMSOURCES = 
SUBOBJS = 

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))

all: $(OBJS)
	$(LD) $(LDFLAGS) -o ../unistd.o $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
