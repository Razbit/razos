CSOURCES = ctype.c math.c portio.c sprintf.c stdlib.c string.c vsprintf.c
ASMSOURCES = 
SUBOBJS = 

OBJS = $(patsubst %.c, %.c.o, $(CSOURCES))
OBJS += $(patsubst %.s, %.s.o, $(ASMSOURCES))

export LDFLAGS += -r

all: $(OBJS) link

link:
	$(LD) $(LDFLAGS) -o lib.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
