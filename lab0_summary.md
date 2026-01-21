# Lab 0 Completion Summary

**Date:** January 21, 2026  
**Environment:** WSL2 (Ubuntu 22.04) on Windows

## Overview
Successfully completed Lab 0: Environment Setup for OSDI (Operating System Development and Implementation). This lab focused on setting up the cross-platform development environment for Raspberry Pi 3 (ARM64/AArch64) bare-metal programming.

---

## Tasks Completed

### 1. Cross Compiler Toolchain Installation ✓
**Status:** COMPLETED

**Toolchain Installed:**
- **Compiler:** `aarch64-linux-gnu-gcc` (Ubuntu 11.4.0-1ubuntu1~22.04) version 11.4.0
- **Linker:** GNU ld (GNU Binutils for Ubuntu) version 2.38
- **Object Copy:** GNU objcopy (GNU Binutils for Ubuntu) version 2.38

**Verification Commands:**
```bash
aarch64-linux-gnu-gcc --version
aarch64-linux-gnu-ld --version
aarch64-linux-gnu-objcopy --version
```

**Notes:**
- The cross compiler toolchain was already present in the WSL2 environment
- All required tools (gcc, ld, objcopy) are functional and ready for ARM64 development

---

### 2. QEMU Installation ⚠️
**Status:** NOT INSTALLED (requires sudo access)

**Required Package:** `qemu-system-arm` or `qemu-system-aarch64`

**Installation Command:**
```bash
sudo apt-get update
sudo apt-get install -y qemu-system-arm
```

**Notes:**
- QEMU is required for emulator testing before deploying to real Raspberry Pi 3 hardware
- Installation requires sudo/root privileges
- While QEMU wasn't installed, the kernel image was successfully built and verified using alternative methods (see section 3)

---

### 3. Kernel Image Build ✓
**Status:** COMPLETED

Successfully built a bootable ARM64 kernel image through the complete toolchain pipeline.

#### Files Created:

**a. Source Files:**

**`a.S`** - ARM64 Assembly Source
```assembly
.section ".text"
_start:
  wfe
  b _start
```
- `wfe`: Wait For Event instruction (low-power idle state)
- `b _start`: Unconditional branch creating an infinite loop

**`linker.ld`** - Linker Script
```ld
SECTIONS
{
  . = 0x80000;
  .text : { *(.text) }
}
```
- Sets load address to `0x80000` (standard RPi3 kernel load address)
- Places all `.text` sections at this location

#### b. Build Process:

**Step 1: Compile to Object File**
```bash
aarch64-linux-gnu-gcc -c a.S
```
**Output:** `a.o` (720 bytes)

**Step 2: Link to ELF Executable**
```bash
aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf a.o
```
**Output:** `kernel8.elf` (65 KB)

**Step 3: Convert to Raw Binary**
```bash
aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
```
**Output:** `kernel8.img` (8 bytes)

#### c. Verification Results:

**Binary Content (hexdump):**
```
00000000  5f 20 03 d5 ff ff ff 17                           |_ ......|
00000008
```

**Disassembly (objdump):**
```
kernel8.elf:     file format elf64-littleaarch64

Disassembly of section .text:

0000000000080000 <_start>:
   80000:	d503205f 	wfe
   80004:	17ffffff 	b	80000 <_start>
```

**Analysis:**
- Instruction at `0x80000`: `d503205f` = `wfe` (Wait For Event)
- Instruction at `0x80004`: `17ffffff` = `b 80000` (branch back to `_start`)
- Total size: 8 bytes (2 instructions × 4 bytes each)
- Load address correctly set to `0x80000`
- Instructions are correctly encoded for ARM64

---

## Project Structure

```
/home/user/osdi/
├── a.S               # ARM64 assembly source code
├── a.o               # Compiled object file (720 bytes)
├── kernel8.elf       # Linked ELF executable (65 KB)
├── kernel8.img       # Raw binary kernel image (8 bytes)
├── lab0.md           # Lab instructions
├── linker.ld         # Linker script
└── lab0_summary.md   # This summary document
```

---

## Key Learnings

### 1. Cross-Platform Development
- Understood the necessity of cross-compilation when developing for different architectures
- Learned that x86_64 host machines require ARM64 cross-compilers to target RPi3
- Recognized that the toolchain includes compiler, assembler, linker, and binary utilities

### 2. Linker Scripts
- Learned that linker scripts control memory layout in bare-metal programming
- Understood the significance of the `0x80000` load address for RPi3
- Recognized that default linker scripts are insufficient for bare-metal development

### 3. Build Pipeline
The complete toolchain pipeline for bare-metal development:
```
Source Code (.S) 
    → [Cross Compiler] → 
Object File (.o) 
    → [Linker + Script] → 
ELF Executable (.elf) 
    → [objcopy] → 
Raw Binary (.img)
```

### 4. ARM64 Instructions
- **WFE (Wait For Event):** Low-power instruction that puts CPU core into standby
- **Branch Instructions:** ARM uses relative addressing for branches
- **Instruction Encoding:** ARM64 uses fixed 32-bit (4-byte) instruction encoding

---

## Testing Strategy

### Without QEMU:
Since QEMU wasn't installed, verification was performed using:
1. **hexdump:** Inspected raw binary to verify correct instruction encoding
2. **objdump:** Disassembled the ELF to verify instructions and addresses
3. **File size verification:** Confirmed 8-byte output matches 2 instructions

### With QEMU (for future use):
Once QEMU is installed, test using:
```bash
qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -d in_asm
```

This will show QEMU's interpretation of the disassembled instructions, confirming proper execution.

---

## Next Steps

### Immediate Actions Required:
1. **Install QEMU:** Run `sudo apt-get install -y qemu-system-arm` to enable emulator testing
2. **Test on QEMU:** Verify kernel boots correctly in emulator environment
3. **Test on Real Hardware:** Deploy `kernel8.img` to actual Raspberry Pi 3

### For Next Labs:
1. **Extend linker script** with additional sections (.data, .bss, stack, etc.)
2. **Add UART functionality** for serial output/debugging
3. **Implement basic kernel services** (memory management, interrupts, etc.)
4. **Set up debugging workflow** using GDB with QEMU

---

## Important Notes

### Warnings from Lab Instructions:
- QEMU's raspi3b machine does NOT perfectly emulate real Raspberry Pi 3 hardware
- Always validate code on actual RPi3 hardware, not just QEMU
- Hardware-specific peripherals may behave differently in emulation

### File Naming Convention:
- `kernel8.img` is the standard name for 64-bit ARM kernels on RPi3
- The "8" indicates AArch64 (64-bit ARM) architecture
- RPi3 bootloader looks for this specific filename

---

## Resources & References

### Commands Reference:
```bash
# Cross compilation
aarch64-linux-gnu-gcc -c source.S
aarch64-linux-gnu-ld -T linker.ld -o output.elf input.o
aarch64-linux-gnu-objcopy -O binary input.elf output.img

# Verification tools
aarch64-linux-gnu-objdump -d file.elf
hexdump -C file.img

# QEMU testing (once installed)
qemu-system-aarch64 -M raspi3b -kernel kernel8.img -display none -d in_asm
```

### Toolchain Information:
- Target Architecture: `aarch64` (ARM 64-bit)
- Target Platform: Raspberry Pi 3 Model B (Broadcom BCM2837)
- CPU: ARM Cortex-A53 (ARMv8-A architecture)
- Default kernel load address: `0x80000`

---

## Conclusion

Lab 0 objectives have been successfully achieved:
- ✓ Development environment is properly configured
- ✓ Cross-platform development workflow is understood
- ✓ First bootable kernel image has been built and verified
- ⚠️ QEMU testing pending installation (requires sudo access)

The toolchain is ready for subsequent labs. The kernel image (`kernel8.img`) can be deployed to Raspberry Pi 3 hardware by copying it to the boot partition of an SD card along with the required RPi3 firmware files.

**Status:** Lab 0 is functionally complete. QEMU installation recommended for enhanced testing workflow in future labs.
