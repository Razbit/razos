CSOURCES = do_exit.c wait.c sched.c setup_rlibc.c
ASMSOURCES = 
SUBOBJS = fcntl.o stdlib.o unistd.o math.o errno.o stat.o ctype.o stdio.o string.o time.o sys.o

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
	$(MAKE) -C stat
	$(MAKE) -C ctype
	$(MAKE) -C stdio
	$(MAKE) -C time
	$(MAKE) -C sys

	$(LD) $(LDFLAGS) -Tuser.ld -o rlibc.o $(CRT0) $(CRTI) $(CRTBEGIN) $(OBJS) $(SUBOBJS) $(CRTEND) $(CRTN)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

test: test.c.o
	$(LD) $(_LDLAGS) -Tuser.ld -o test rlibc.o test.c.o

hellow: hellow.c.o
	$(LD) $(_LDLAGS) -Tuser.ld -o hellow rlibc.o hellow.c.o

rash: rash.c.o
	$(LD) $(_LDLAGS) -Tuser.ld -o rash rlibc.o rash.c.o

len: len.c.o
	$(LD) $(_LDLAGS) -Tuser.ld -o len rlibc.o len.c.o

term: term.c.o
	$(LD) $(_LDLAGS) -Tuser.ld -o term rlibc.o term.c.o

clean:
	rm *.o
	$(MAKE) clean -C arch
	$(MAKE) clean -C fcntl
	$(MAKE) clean -C stdlib
	$(MAKE) clean -C unistd
	$(MAKE) clean -C math
	$(MAKE) clean -C string
	$(MAKE) clean -C errno
	$(MAKE) clean -C stat
	$(MAKE) clean -C ctype
	$(MAKE) clean -C stdio
	$(MAKE) clean -C time
	$(MAKE) clean -C sys

