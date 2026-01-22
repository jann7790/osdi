# 如何在 Raspberry Pi 3 上印出 Hello World

## 概述

本文件說明如何在裸機（bare-metal）環境下，透過 ARM64 組合語言在 Raspberry Pi 3 上印出 "Hello World"。

## 修改內容

### 1. 修改 `a.S` - 主要程式碼

原本的 `a.S` 只有簡單的無限迴圈：
```assembly
_start:
  wfe
  b _start
```

現在修改成完整的 UART 輸出程式，包含以下部分：

#### 1.1 定義 UART 暫存器位址
```assembly
.equ UART_BASE,   0x3F201000  // PL011 UART 基底位址
.equ UART_DR,     0x3F201000  // 資料暫存器
.equ UART_FR,     0x3F201018  // 旗標暫存器
```

**為什麼使用 PL011 UART？**
- Raspberry Pi 3 有兩個 UART：PL011 (主要) 和 Mini UART (輔助)
- QEMU 的 raspi3b 模型預設將 PL011 連接到序列埠
- PL011 在 QEMU 中已經被韌體初始化好了，不需要額外設定

#### 1.2 主程式 `_start`
```assembly
_start:
    ldr x0, =hello_msg    // 載入字串位址
    bl uart_puts          // 呼叫印出字串函式
hang:
    wfe                   // 等待事件（省電）
    b hang                // 無限迴圈
```

#### 1.3 實作 `uart_putc` - 印出單一字元
```assembly
uart_putc:
    ldr x1, =UART_FR
wait_txready:
    ldr w2, [x1]
    and w2, w2, #0x20         // 檢查 TXFF（傳送 FIFO 已滿）旗標
    cbnz w2, wait_txready     // 如果滿了就等待
    
    ldr x1, =UART_DR
    str w0, [x1]              // 寫入字元到資料暫存器
    ret
```

**運作原理：**
1. 讀取 UART 旗標暫存器 (UART_FR)
2. 檢查 bit 5 (TXFF) - 如果是 1 表示傳送 FIFO 已滿
3. 等待直到 FIFO 有空間
4. 將字元寫入 UART 資料暫存器 (UART_DR)

#### 1.4 實作 `uart_puts` - 印出字串
```assembly
uart_puts:
    mov x19, x0               // 儲存字串指標
    mov x20, x30              // 儲存返回位址
next_char:
    ldrb w0, [x19], #1        // 載入一個位元組，並遞增指標
    cbz w0, puts_done         // 如果是 null (0)，結束
    bl uart_putc              // 印出字元
    b next_char               // 繼續下一個字元
puts_done:
    mov x30, x20              // 恢復返回位址
    ret
```

**運作原理：**
1. 逐個讀取字串中的字元（以位元組為單位）
2. 如果遇到 null 終止符號 (`\0`)，結束
3. 否則呼叫 `uart_putc` 印出該字元
4. 使用 post-index addressing：`[x19], #1` 會自動將指標加 1

#### 1.5 資料區段 - 字串資料
```assembly
.section ".data"
hello_msg:
    .asciz "Hello World\n"
```

`.asciz` 會自動在字串結尾加上 null 終止符號。

### 2. 修改 `linker.ld` - 連結器腳本

原本只有 `.text` 區段：
```ld
SECTIONS
{
  . = 0x80000;
  .text : { *(.text) }
}
```

修改後加入 `.data` 和 `.bss` 區段：
```ld
SECTIONS
{
  . = 0x80000;
  .text : { *(.text) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
```

**為什麼需要修改？**
- `.data` 區段用來存放已初始化的全域變數（我們的 "Hello World\n" 字串）
- `.bss` 區段用來存放未初始化的全域變數
- 沒有這些定義，連結器不知道要把資料放在哪裡

### 3. 修改 `Makefile` - 建置腳本

將 `run` 目標從：
```makefile
run: kernel8_lab0.img
	qemu-system-aarch64 -M raspi3b -kernel kernel8_lab0.img -display none -d in_asm
```

改成：
```makefile
run: kernel8_lab0.img
	qemu-system-aarch64 -M raspi3b -kernel kernel8_lab0.img -serial stdio -display none
```

**修改原因：**
- `-serial stdio`：將 UART 輸出導向到標準輸出/輸入
- 移除 `-d in_asm`：不需要顯示組合語言除錯資訊
- 這樣才能在終端機看到 "Hello World" 輸出

## 技術細節

### 記憶體映射 I/O (MMIO)

Raspberry Pi 使用記憶體映射 I/O 來控制硬體：
- UART 暫存器被映射到特定的記憶體位址
- 透過 `ldr` (load) 和 `str` (store) 指令來讀寫暫存器
- 位址 0x3F201000 是 BCM2837 晶片中 PL011 UART 的實體位址

### 暫存器使用慣例

- `x0-x7`：函式參數和返回值
- `x19-x28`：被呼叫者保存暫存器（callee-saved）
- `x30` (LR)：連結暫存器，儲存返回位址
- 在 `uart_puts` 中使用 `x19` 和 `x20` 來保存值，符合 ARM64 呼叫慣例

### 為什麼不需要初始化 UART？

在實體 Raspberry Pi 上，需要：
1. 設定 GPIO 腳位為 UART 模式
2. 設定鮑率（baud rate）
3. 啟用 UART 傳送/接收

但在 QEMU 中：
- QEMU 的韌體已經幫我們初始化好 PL011 UART
- 可以直接使用 UART_DR 和 UART_FR 暫存器
- 這大幅簡化了程式碼（從 150 行減少到 40 行）

## 如何執行

### 建置並執行
```bash
make clean
make
make run
```

### 預期輸出
```
Hello World
```

程式會印出 "Hello World" 後進入無限迴圈。按 `Ctrl+C` 可以結束 QEMU。

## 除錯技巧

### 查看組合後的程式碼
```bash
aarch64-linux-gnu-objdump -d kernel8_lab0.elf
```

### 查看 ELF 檔案結構
```bash
aarch64-linux-gnu-readelf -a kernel8_lab0.elf
```

### 檢查記憶體佈局
```bash
aarch64-linux-gnu-nm kernel8_lab0.elf
```

## 總結

為了在裸機環境印出 "Hello World"，我們需要：

1. **定義 UART 暫存器位址**：知道硬體的記憶體位址
2. **實作字元輸出函式**：透過 MMIO 操作 UART
3. **實作字串輸出函式**：逐字元印出直到遇到 null
4. **準備字串資料**：在 `.data` 區段定義字串
5. **更新連結器腳本**：確保資料區段被正確連結
6. **設定 QEMU 參數**：讓 UART 輸出導向終端機

整個過程展示了低階程式設計的核心概念：直接操作硬體暫存器、記憶體管理、以及組合語言程式設計技巧。
