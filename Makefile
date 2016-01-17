# set these to your cross compiler (see cross-compiler for instructions)
export CC = i686-elf-gcc 
export LD = i686-elf-ld

all:
	echo "Usage: make kernel|rlibc|init|clean|bochs|qemu|tools"

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
.PHONY: init
init:
	$(MAKE) -C rlibc
	$(MAKE) test -C rlibc
	$(MAKE) hellow -C rlibc
	mv rlibc/test ./test
	mv rlibc/hellow ./hellow
	tools/mk-initrd test hellow

bochs:
	sudo ./update_image.sh
	sudo ./run_bochs.sh

qemu:
	sudo ./update_image.sh
	qemu-system-x86_64 -fda floppy.img -m 512 -d cpu_reset
