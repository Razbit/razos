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
