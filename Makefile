CROSS_COMPILE = aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -T linker.ld

SOURCES_ASM = start.S
SOURCES_C = main.c uart.c shell.c mailbox.c

OBJECTS = $(SOURCES_ASM:.S=.o) $(SOURCES_C:.c=.o)

TARGET = kernel8

.PHONY: all clean run

all: $(TARGET).img

$(TARGET).img: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET).elf $(TARGET).img

run: $(TARGET).img
	qemu-system-aarch64 -M raspi3b -kernel $(TARGET).img -serial stdio

debug: $(TARGET).elf
	qemu-system-aarch64 -M raspi3b -kernel $(TARGET).img -serial stdio -d int
