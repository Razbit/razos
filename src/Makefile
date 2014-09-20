SOURCES=boot.o main.o monitor.o
LIBS=lib/common.o lib/math.o lib/string.o

CFLAGS=-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector
LDFLAGS=-m elf_i386 -Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) $(LIBS) link 

clean:
	-rm *.o kernel lib/*.o

link:
	ld $(LDFLAGS) -o kernel $(SOURCES) $(LIBS)

.s.o:
	nasm $(ASFLAGS) $<

