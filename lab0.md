Lab 0: Environment Setup
Introduction
In Lab 0, you need to prepare the environment for future development. You should install the target toolchain, and use them to build a bootable image for rpi3.

Goals of this lab
Set up the development environment.

Understand what’s cross-platform development.

Test your rpi3.

Important

This lab is an introductory lab. It won’t be part of your final grade, but you still need to finish all todo parts, or you’ll be in trouble in the next lab.

Cross-Platform Development
Cross Compiler
Rpi3 uses ARM Cortex-A53 CPU. To compile your source code to 64-bit ARM machine code, you need a cross compiler if you develop on a non-ARM64 environment.

Todo

Install a cross compiler on your host computer.

Linker
You might not notice the existence of linkers before. It’s because the compiler uses the default linker script for you. (ld --verbose to check the content) In bare-metal programming, you should set the memory layout yourself.

This is an incomplete linker script for you. You should extend it in the following lab.

SECTIONS
{
  . = 0x80000;
  .text : { *(.text) }
}
QEMU
In cross-platform development, it’s easier to validate your code on an emulator first. You can use QEMU to test your code first before validating them on a real rpi3.

Warning

Although QEMU provides a machine option for rpi3, it doesn’t behave the same as a real rpi3. You should validate your code on your rpi3, too.

Todo

Install qemu-system-aarch64.

From Source Code to Kernel Image
You have the basic knowledge of the toolchain for cross-platform development. Now, it’s time to practice them.

From Source Code to Object Files
Source code is converted to object files by a cross compiler. After saving the following assembly as a.S, you can convert it to an object file by aarch64-linux-gnu-gcc -c a.S. Or if you would like to, you can also try llvm’s linker clang -mcpu=cortex-a53 --target=aarch64-rpi3-elf -c a.S, especially if you are trying to develop on macOS.

.section ".text"
_start:
  wfe
  b _start
From Object Files to ELF
A linker links object files to an ELF file. An ELF file can be loaded and executed by program loaders. Program loaders are usually provided by the operating system in a regular development environment. In bare-metal programming, ELF can be loaded by some bootloaders.

To convert the object file from the previous step to an ELF file, you can save the provided linker script as linker.ld, and run the following command.

# On GNU LD
aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf a.o
# On LLVM
ld.lld -m aarch64elf -T linker.ld -o kernel8.elf a.o
From ELF to Kernel Image
Rpi3’s bootloader can’t load ELF files. Hence, you need to convert the ELF file to a raw binary image. You can use objcopy to convert ELF files to raw binary.

aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
# Or
llvm-objcopy --output-target=aarch64-rpi3-elf -O binary kernel8.elf kernle8.img
Check on QEMU
After building, you can use QEMU to see the dumped assembly.

qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -d in_asm
Todo

Build your first kernel image, and check it on QEMU.