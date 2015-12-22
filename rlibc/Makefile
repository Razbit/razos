CSOURCES = test.c do_exit.c wait.c sched.c
ASMSOURCES = 
SUBOBJS = fcntl.o stdlib.o unistd.o math.o errno.o

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))

export PWD = $(shell pwd)
export CFLAGS = -std=gnu99 -O2 -Wall -Wextra -c -g \
	-I$(PWD) -I$(PWD)/../razos_kernel_headers
export LDFLAGS = -r
export ASFLAGS = -felf
export ARCH = i386

_LDFLAGS = 

CRT0 = $(PWD)/arch/crt0.s.o
CRTI = $(PWD)/arch/crti.s.o
CRTBEGIN = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN = $(PWD)/arch/crtn.s.o

all: $(OBJS)
	$(MAKE) -C arch
	$(MAKE) -C fcntl
	$(MAKE) -C stdlib
	$(MAKE) -C unistd
	$(MAKE) -C math
	$(MAKE) -C string
	$(MAKE) -C errno

	$(LD) $(_LDFLAGS) -Tuser.ld -o rlibc $(CRT0) $(CRTI) $(CRTBEGIN) $(OBJS) $(SUBOBJS) $(CRTEND) $(CRTN)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
	$(MAKE) clean -C arch
	$(MAKE) clean -C fcntl
	$(MAKE) clean -C stdlib
	$(MAKE) clean -C unistd
	$(MAKE) clean -C math
	$(MAKE) clean -C string
	$(MAKE) clean -C errno

