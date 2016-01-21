CSOURCES = memchr.c memcmp.c memcpy.c memmove.c memset.c strcat.c strchr.c strcmp.c strcpy.c strlen.c strpbrk.c strspn.c strstr.c strtok.c strerror.c
ASMSOURCES = 
SUBOBJS = 

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))

all: $(OBJS)
	$(LD) $(LDFLAGS) -o ../string.o $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
