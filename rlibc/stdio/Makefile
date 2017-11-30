CSOURCES = vdprintf.c printf.c puts.c dprintf.c dgetc.c getchar.c vsprintf.c sprintf.c putc.c dgetdelim.c dgetline.c
ASMSOURCES =
SUBOBJS =

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))

all: $(OBJS)
	$(LD) $(LDFLAGS) -o ../stdio.o $(OBJS)

%.s.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
