SOURCES=kern/boot.s.o kern/kmain.o kern/kio.o kern/vga.o kern/isr.o \
kern/descriptor_tables.o kern/gdt.o kern/gdt.s.o kern/idt.o \
kern/idt.s.o kern/interrupt.s.o

LIBS=lib/portio.o lib/math.o lib/string.o lib/ctype.o lib/vsprintf.o \
lib/stdlib.o lib/sprintf.o

CFLAGS=-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -g
LDFLAGS=-m elf_i386 -Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) $(LIBS) link 

clean:
	rm kern/*.o lib/*.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES) $(LIBS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

