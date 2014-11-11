CSOURCES = idt.c irq.c isr.c timer.c
ASMSOURCES = idt.s interrupt.s
SUBOBJS = 

OBJS = $(patsubst %.c,%.c.o,$(CSOURCES))
OBJS += $(patsubst %.s,%.s.o,$(ASMSOURCES))


all: $(OBJS) link 

link:
	ld $(LDFLAGS) -o interrupt.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	gcc $(CFLAGS) $< -o $@

clean:
	rm *.o
