CSOURCES = ctype.c portio.c sprintf.c stdlib.c string.c vsprintf.c fcntl.c unistd.c errno.c mktime.c
ASMSOURCES = 
SUBOBJS = 

OBJS = $(patsubst %.c, %.c.o, $(CSOURCES))
OBJS += $(patsubst %.s, %.s.o, $(ASMSOURCES))

export LDFLAGS += -r

all: $(OBJS) link

link:
	$(LD) $(LDFLAGS) -o lib.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
