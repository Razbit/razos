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
	$(MAKE) date -C rlibc
	$(MAKE) pipe_test -C rlibc
	mv rlibc/test ./test
	mv rlibc/hellow ./hellow
	mv rlibc/rash ./rash
	mv rlibc/len ./len
	mv rlibc/term ./term
	mv rlibc/date ./date
	mv rlibc/pipe_test ./pipe_test
	tools/mk-initrd test hellow rash len term date pipe_test

bochs:
	sudo ./update_image.sh
	sudo ./run_bochs.sh

qemu:
	sudo ./update_image.sh
	qemu-system-x86_64 -fda floppy.img -drive format=raw,file=hd0.img -boot order=a -m 512 -d cpu_reset
