SOURCES=kern/boot.o kern/kmain.o kern/kio.o kern/vga.o
LIBS=lib/portio.o lib/math.o lib/string.o lib/ctype.o lib/vsprintf.o lib/stdlib.o

CFLAGS=-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector
LDFLAGS=-m elf_i386 -Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) $(LIBS) link 

clean:
	rm kern/*.o lib/*.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES) $(LIBS)

.s.o:
	nasm $(ASFLAGS) $<

