SUBOBJS = kern.o lib.o

export PWD = $(shell pwd)
export CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c -g \
	-I$(PWD)/include -I$(PWD)/../razos_kernel_headers
export LDFLAGS = 
export ASFLAGS = -felf
export ARCH = i386

CRTI = $(PWD)/arch/crti.s.o
CRTBEGIN = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN = $(PWD)/arch/crtn.s.o

all: ./src/boot.s.o
	$(MAKE) -C lib
	mv lib/lib.o ./lib.o

	$(MAKE) -C src
	mv src/kern.o ./kern.o

	$(MAKE) -C arch

	$(LD) $(LDFLAGS) -Tlink.ld -o kernel $(CRTI) $(CRTBEGIN) ./src/boot.s.o $(SUBOBJS) $(CRTEND) $(CRTN)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

clean:
	$(MAKE) clean -C lib
	$(MAKE) clean -C src
	$(MAKE) clean -C arch
	rm ./*.o kernel


