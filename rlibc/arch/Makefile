CSOURCES = 
ASMSOURCES = $(ARCH)/crti.s $(ARCH)/crtn.s $(ARCH)/crt0.s
SUBOBJS = 

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))


all: $(OBJS)
	cp $(ARCH)/crti.s.o ./crti.s.o
	cp $(ARCH)/crtn.s.o ./crtn.s.o
	cp $(ARCH)/crt0.s.o ./crt0.s.o

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
