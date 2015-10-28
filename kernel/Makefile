SUBOBJS = kern.o lib.o

export PWD = $(shell pwd)
export CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c -g \
	-I$(PWD)/include -I$(PWD)/../razos_kernel_headers
export LDFLAGS =
export ASFLAGS = -felf

all: ./src/boot.s.o
	$(MAKE) -C lib
	mv lib/lib.o ./lib.o

	$(MAKE) -C src
	mv src/kern.o ./kern.o

	$(LD) $(LDFLAGS) -Tlink.ld -o kernel ./src/boot.s.o $(SUBOBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

clean:
	$(MAKE) clean -C lib
	$(MAKE) clean -C src
	rm ./*.o kernel


