SUBOBJS = kern.o lib.o

export PWD = $(shell pwd)
export CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c -g \
	-I$(PWD)/include -I$(PWD)/../razos_kernel_headers -D_RAZOS_KERNEL
export LDFLAGS =
export ASFLAGS =

all: ./src/boot.s.o
	$(MAKE) -C lib
	mv lib/lib.o ./lib.o

	$(MAKE) -C src
	mv src/kern.o ./kern.o

	$(LD) $(LDFLAGS) -Tlink.ld -o kernel ./src/boot.s.o $(SUBOBJS)

%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	$(MAKE) clean -C lib
	$(MAKE) clean -C src
	rm ./*.o kernel


