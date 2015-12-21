CSOURCES = ceil.c  fabs.c  floor.c  fmod.c  frexp.c  ldexp.c modf.c  sqrt.c  trunc.c
ASMSOURCES = 
SUBOBJS = 

OBJS = $(patsubst %.c, %.c.o,$(CSOURCES))
OBJS += $(patsubst %.s, %.s.o,$(ASMSOURCES))

all: $(OBJS)
	$(LD) $(LDFLAGS) -o ../math.o $(OBJS)

%.s.o: %.s
	nasm $(ASFLAGS) $< -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
