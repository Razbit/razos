all:
	$(MAKE) -C src

clean:
	$(MAKE) clean -C src

bochs:
	sudo ./update_image.sh
	sudo ./run_bochs.sh

qemu:
	sudo ./update_image.sh
	qemu-system-x86_64 -fda floppy.img
