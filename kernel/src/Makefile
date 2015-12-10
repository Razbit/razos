CSOURCES = gdt.c console.c kmain.c vga.c panic.c
ASMSOURCES = gdt.s
SUBOBJS = interrupt.o mm.o syscall.o fs.o

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))

export LDFLAGS += -r

all: $(OBJS)
	$(MAKE) -C interrupt
	mv interrupt/interrupt.o ./interrupt.o

	$(MAKE) -C mm
	mv mm/mm.o ./mm.o

	$(MAKE) -C syscall
	mv syscall/syscall.o ./syscall.o

	$(MAKE) -C fs
	mv fs/fs.o ./fs.o

	$(LD) $(LDFLAGS) -o kern.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	$(MAKE) clean -C interrupt
	$(MAKE) clean -C mm
	$(MAKE) clean -C syscall
