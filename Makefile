all: kernel8_lab0.img

a.o: a.S
	aarch64-linux-gnu-gcc -c a.S -o a.o

kernel8_lab0.elf: a.o linker.ld
	aarch64-linux-gnu-ld -T linker.ld -o kernel8_lab0.elf a.o

kernel8_lab0.img: kernel8_lab0.elf
	aarch64-linux-gnu-objcopy -O binary kernel8_lab0.elf kernel8_lab0.img

run: kernel8_lab0.img
	qemu-system-aarch64 -M raspi3b -kernel kernel8_lab0.img -serial stdio -display none

clean:
	rm -f a.o kernel8_lab0.elf kernel8_lab0.img

.PHONY: all run clean
