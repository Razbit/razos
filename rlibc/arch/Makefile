CSOURCES = 
ASMSOURCES = $(ARCH)/crti.s $(ARCH)/crtn.s $(ARCH)/crt0.s
SUBOBJS = 

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))


all: $(OBJS)
	mv $(ARCH)/crti.s.o ./crti.s.o
	mv $(ARCH)/crtn.s.o ./crtn.s.o
	mv $(ARCH)/crt0.s.o ./crt0.s.o

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
