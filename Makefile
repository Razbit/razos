# set these to your cross compiler (see cross-compiler for instructions)
export CC = i686-elf-gcc 
export LD = i686-elf-ld

all:
	echo "Usage: make kernel|rlibc|clean|bochs|qemu|tools"

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

.PHONY: rlibc
rlibc:
	$(MAKE) -C rlibc

clean:
	$(MAKE) clean -C kernel
	$(MAKE) clean -C rlibc

bochs:
	sudo ./update_image.sh
	sudo ./run_bochs.sh

qemu:
	sudo ./update_image.sh
	qemu-system-x86_64 -fda floppy.img

tools:
	$(MAKE) tools -C tools

