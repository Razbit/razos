# set these to your cross compiler (see cross-compiler for instructions)
export CC = i686-elf-gcc 
export LD = i686-elf-ld

all:
	echo "Usage: make kernel|clean|bochs|qemu|tools"

kernel:
	$(MAKE) -C kernel

clean:
	$(MAKE) clean -C kernel

bochs:
	sudo ./update_image.sh
	sudo ./run_bochs.sh

qemu:
	sudo ./update_image.sh
	qemu-system-x86_64 -fda floppy.img

tools:
	$(MAKE) tools -C tools

.PHONY: kernel
