# OSDI Lab 0

## 快速開始

```bash
# 1. 安裝工具鏈
./setup.sh

# 2. 編譯並執行
make -f Makefile.lab0 run

# 3. 清理
make -f Makefile.lab0 clean
```

## 檔案說明

### 核心檔案
- **a.S** - ARM64 組合語言源碼
- **linker_lab0.ld** - 連結器腳本（設定記憶體從 0x80000 開始）

### 工具
- **Makefile.lab0** - 自動化編譯腳本
- **setup.sh** - 環境設定腳本

## 安裝工具鏈

### 自動安裝
```bash
./setup.sh
```

### 手動安裝
```bash
sudo apt-get update
sudo apt-get install -y gcc-aarch64-linux-gnu qemu-system-aarch64
```

### 驗證安裝
```bash
aarch64-linux-gnu-gcc --version
qemu-system-aarch64 --version
```

## 手動編譯步驟

如果你想了解編譯過程：

```bash
# 步驟 1: 編譯組合語言成目標檔
aarch64-linux-gnu-gcc -c a.S -o a.o

# 步驟 2: 連結目標檔成 ELF
aarch64-linux-gnu-ld -T linker_lab0.ld -o kernel8_lab0.elf a.o

# 步驟 3: 轉換 ELF 成映像檔
aarch64-linux-gnu-objcopy -O binary kernel8_lab0.elf kernel8_lab0.img

# 步驟 4: 在 QEMU 上測試
qemu-system-aarch64 -M raspi3b -kernel kernel8_lab0.img -display none -d in_asm
```

## 預期輸出

執行 `make -f Makefile.lab0 run` 後，你會看到很多輸出，**找到這兩行就對了**：

```
IN: 
0x0000000000080000:  d503205f      unimplemented (System)
0x0000000000080004:  17ffffff      b #-0x4 (addr 0x80000)
```

這就是你的程式：
- `0x80000` - 你的 `wfe` 指令（QEMU 顯示為 "unimplemented (System)"）
- `0x80004` - 你的 `b _start` 無限循環

其他位址（0x300, 0x0 等）的指令是 Raspberry Pi 3 的 bootloader，用來初始化硬體和載入你的 kernel。這是**正常現象**！

## 程式說明

### a.S
```asm
.section ".text"
_start:
  wfe           # Wait For Event - CPU 進入低功耗狀態
  b _start      # 無限循環
```

### linker_lab0.ld
```ld
SECTIONS
{
  . = 0x80000;              # 設定載入位址為 0x80000
  .text : { *(.text) }      # 定義代碼段
}
```

## 故障排除

### 找不到 aarch64-linux-gnu-gcc
```bash
sudo apt-get install gcc-aarch64-linux-gnu
```

### 找不到 qemu-system-aarch64
```bash
sudo apt-get install qemu-system-aarch64
```

### macOS 上開發
```bash
brew install aarch64-elf-gcc qemu
```
然後修改 Makefile.lab0 中的 `CROSS_COMPILE = aarch64-elf-`

## 詳細解說

想了解 Lab 0 背後的原理嗎？請看 **LAB0_EXPLAINED.md**，裡面包含：
- 什麼是交叉編譯？
- 完整的編譯流程詳解
- 什麼是裸機程式設計？
- 記憶體佈局解析
- 為什麼是 0x80000？
- QEMU 的作用

## 注意事項

- Lab 0 檔案使用 `_lab0` 後綴，不會影響其他 lab 的檔案
- 原有的 `Makefile` 和 `linker.ld` 是給其他 lab 使用的
