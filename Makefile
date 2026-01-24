CROSS_COMPILE = aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib

OBJS = start.o uart.o mailbox.o main.o

all: kernel8.img

start.o: start.S
	$(CC) $(CFLAGS) -c start.S -o start.o

uart.o: uart.c uart.h
	$(CC) $(CFLAGS) -c uart.c -o uart.o

mailbox.o: mailbox.c mailbox.h
	$(CC) $(CFLAGS) -c mailbox.c -o mailbox.o

main.o: main.c uart.h mailbox.h
	$(CC) $(CFLAGS) -c main.c -o main.o

kernel8.elf: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -T linker.ld -o kernel8.elf $(OBJS)

kernel8.img: kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

run: kernel8.img
	qemu-system-aarch64 -M raspi3b -kernel kernel8.img -serial stdio -display none

clean:
	rm -f *.o kernel8.elf kernel8.img kernel8_lab0.elf kernel8_lab0.img a.o

.PHONY: all run clean
