SUBOBJS = kern.o lib.o

export PWD = $(shell pwd)
export CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -c -g -I$(PWD)/include
export LDFLAGS = -m elf_i386
export ASFLAGS = -felf

all: boot.s.o
	$(MAKE) -C lib
	mv lib/lib.o ./lib.o

	$(MAKE) -C kern
	mv kern/kern.o ./kern.o

	ld $(LDFLAGS) -Tlink.ld -o kernel boot.s.o $(SUBOBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

clean:
	rm ./*.o kernel
	$(MAKE) clean -C lib
	$(MAKE) clean -C kern

tools:
	$(MAKE) all -C tools
	mv tools/mk-initrd ../mk-initrd
	mv tools/hexdump ../hexdump

.PHONY: tools

