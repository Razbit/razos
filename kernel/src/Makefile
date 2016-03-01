CSOURCES = gdt.c console.c kmain.c vga.c panic.c power.c
ASMSOURCES = gdt.s
SUBOBJS = drivers.o mm.o syscall.o fs.o loader.o

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))

export LDFLAGS += -r

all: $(OBJS)
	$(MAKE) -C drivers
	mv drivers/drivers.o ./drivers.o

	$(MAKE) -C mm
	mv mm/mm.o ./mm.o

	$(MAKE) -C syscall
	mv syscall/syscall.o ./syscall.o

	$(MAKE) -C fs
	mv fs/fs.o ./fs.o

	$(MAKE) -C loader
	mv loader/loader.o ./loader.o

	$(LD) $(LDFLAGS) -o kern.o $(SUBOBJS) $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	$(MAKE) clean -C drivers
	$(MAKE) clean -C mm
	$(MAKE) clean -C syscall
	$(MAKE) clean -C fs
	$(MAKE) clean -C loader

