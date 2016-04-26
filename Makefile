# set these to your cross compiler (see cross-compiler for instructions)
export CC = i686-elf-gcc
export LD = i686-elf-ld

all: tools kernel rlibc init

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

.PHONY: rlibc
rlibc:
	$(MAKE) -C rlibc

.PHONY: tools
tools:
	$(MAKE) -C tools

.PHONY: clean
clean:
	$(MAKE) clean -C kernel
	$(MAKE) clean -C rlibc

init: rlibc
	$(MAKE) test -C rlibc
	$(MAKE) hellow -C rlibc
	$(MAKE) rash -C rlibc
	$(MAKE) len -C rlibc
	$(MAKE) term -C rlibc
	mv rlibc/test ./test
	mv rlibc/hellow ./hellow
	mv rlibc/rash ./rash
	mv rlibc/len ./len
	mv rlibc/term ./term
	tools/mk-initrd test hellow rash len term

bochs:
	sudo ./update_image.sh
	sudo ./run_bochs.sh

qemu:
	sudo ./update_image.sh
	qemu-system-x86_64 -fda floppy.img -m 512 -d cpu_reset
