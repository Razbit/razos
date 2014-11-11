CSOURCES = gdt.c kio.c kmain.c vga.c
ASMSOURCES = gdt.s 
SUBOBJS = interrupt.o

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))

export LDFLAGS += -r

all: $(OBJS)
	$(MAKE) -C interrupt
	mv interrupt/interrupt.o ./interrupt.o

	ld $(LDFLAGS) -o kern.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	gcc $(CFLAGS) $< -o $@

clean:
	rm *.o
	$(MAKE) clean -C interrupt
