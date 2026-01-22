# Lab 0 詳細解說

## 目標

Lab 0 是一個入門實驗，目的是：
1. 建立交叉編譯環境
2. 理解從源碼到可執行映像檔的完整流程
3. 學習裸機（bare-metal）程式設計的基礎

## 什麼是交叉編譯（Cross Compilation）？

### 問題背景
- **你的電腦**：可能是 x86_64 架構（Intel/AMD CPU）
- **Raspberry Pi 3**：是 ARM64 架構（ARM Cortex-A53 CPU）

你不能直接用普通的 `gcc` 編譯 ARM64 程式，因為普通 gcc 只會產生 x86_64 的機器碼。

### 解決方案：交叉編譯器
- **aarch64-linux-gnu-gcc**：在 x86_64 電腦上執行，但產生 ARM64 機器碼
- 這樣你就可以在電腦上開發，然後把程式放到 Raspberry Pi 上執行

---

## 完整的編譯流程

Lab 0 教你把一個簡單的組合語言程式變成可開機的映像檔：

```
a.S (組合語言)
    ↓ [編譯器]
a.o (目標檔/機器碼)
    ↓ [連結器]
kernel8.elf (ELF 可執行檔)
    ↓ [objcopy]
kernel8.img (原始二進位映像檔)
    ↓ [bootloader]
在 Raspberry Pi 上執行！
```

### 步驟 1: 組合語言 → 目標檔 (a.S → a.o)

**檔案：a.S**
```asm
.section ".text"    # 告訴編譯器：這是程式碼段
_start:             # 定義程式進入點標籤
  wfe               # Wait For Event - CPU 休眠，節省電力
  b _start          # 跳回 _start，無限循環
```

**執行命令：**
```bash
aarch64-linux-gnu-gcc -c a.S -o a.o
```

**這步驟做了什麼？**
- 把組合語言翻譯成 ARM64 機器碼
- 產生 `a.o` 目標檔（object file）
- 這個檔案還不能直接執行，因為沒有指定記憶體位址

**查看機器碼：**
```bash
aarch64-linux-gnu-objdump -d a.o
```
你會看到：
```
0: d503205f    wfe
4: 17ffffff    b 0 <_start>
```

### 步驟 2: 目標檔 → ELF 可執行檔 (a.o → kernel8.elf)

**問題：**
- 目標檔包含機器碼，但不知道要載入到哪個記憶體位址
- Raspberry Pi 3 規定程式必須從 **0x80000** 開始執行

**解決方案：連結器腳本 (linker_lab0.ld)**
```ld
SECTIONS
{
  . = 0x80000;              # 設定起始位址為 0x80000
  .text : { *(.text) }      # 把所有 .text 段放在這裡
}
```

**執行命令：**
```bash
aarch64-linux-gnu-ld -T linker_lab0.ld -o kernel8_lab0.elf a.o
```

**這步驟做了什麼？**
- 把機器碼放到正確的記憶體位址
- 產生 ELF 格式的可執行檔
- ELF (Executable and Linkable Format) 包含很多元資料（metadata）

**為什麼是 0x80000？**
- Raspberry Pi 3 的 GPU bootloader 會把 kernel 載入到 0x80000
- 0x0 ~ 0x80000 這段記憶體給 GPU 和 bootloader 使用
- 這是硬體規定，不能改變

### 步驟 3: ELF → 原始映像檔 (kernel8.elf → kernel8.img)

**問題：**
- Raspberry Pi 的 bootloader 不懂 ELF 格式
- 它只能載入純粹的二進位資料（raw binary）

**執行命令：**
```bash
aarch64-linux-gnu-objcopy -O binary kernel8_lab0.elf kernel8_lab0.img
```

**這步驟做了什麼？**
- 移除所有 ELF 的元資料（檔頭、符號表等）
- 只留下純機器碼
- 產生 `kernel8_lab0.img` - 這就是可開機的映像檔！

**為什麼叫 kernel8？**
- kernel7.img = 32-bit ARM
- kernel8.img = 64-bit ARM (ARMv8)
- bootloader 會根據檔名選擇正確的模式

### 步驟 4: 在 QEMU 上測試

**執行命令：**
```bash
qemu-system-aarch64 -M raspi3b -kernel kernel8_lab0.img -display none -d in_asm
```

**參數說明：**
- `-M raspi3b`：模擬 Raspberry Pi 3B 硬體
- `-kernel kernel8_lab0.img`：載入你的映像檔
- `-display none`：不顯示圖形介面
- `-d in_asm`：顯示執行的組合語言指令

**輸出說明：**
```
IN: 
0x0000000000080000:  d503205f      unimplemented (System)   # 這是你的 wfe
0x0000000000080004:  17ffffff      b #-0x4 (addr 0x80000)   # 這是你的 b _start
```

---

## 什麼是裸機程式設計（Bare-Metal Programming）？

### 一般程式 vs 裸機程式

| 項目 | 一般程式（如：在 Linux 上） | 裸機程式（Lab 0） |
|------|---------------------------|-----------------|
| 作業系統 | 有（Linux/Windows） | 無 |
| 啟動方式 | OS 載入程式 | bootloader 載入 |
| 記憶體管理 | OS 自動分配 | 你自己管理 |
| 系統呼叫 | 可用 printf, malloc | 都沒有！ |
| 硬體存取 | 透過驅動程式 | 直接存取硬體暫存器 |

### 在 Lab 0 中：
- **沒有作業系統**，你的程式就是作業系統！
- **沒有 printf**，要自己寫 UART 驅動來輸出文字（Lab 1）
- **沒有 malloc**，要自己管理記憶體（後續 Lab）
- **直接控制硬體**，包括 CPU、記憶體、週邊設備

---

## 記憶體佈局

Raspberry Pi 3 的記憶體配置：

```
0x00000000 - 0x00080000    GPU 保留區域 & Bootloader
0x00080000 - ...           你的 Kernel 程式碼從這裡開始
                           ↑
                           這就是 linker script 設定的位址
```

當 Raspberry Pi 開機時：
1. GPU 先啟動，執行 bootloader
2. Bootloader 從 SD 卡讀取 `kernel8.img`
3. 把它載入到記憶體 0x80000
4. 跳轉到 0x80000 開始執行你的程式
5. 此時 CPU 接手，開始執行你的第一條指令 `wfe`

---

## 你的程式在做什麼？

```asm
.section ".text"
_start:
  wfe               # Wait For Event
  b _start          # 跳回 _start
```

### wfe 指令
- **Wait For Event** - 讓 CPU 進入低功耗模式
- CPU 會休眠，直到收到中斷或事件
- 省電！不會讓 CPU 一直空轉

### b _start 指令
- **無條件跳轉** 回到 `_start`
- 形成無限循環
- 這樣 CPU 就不會跑到未知的記憶體位址

### 為什麼這樣寫？
因為這是最簡單的 kernel：
- 開機後不做任何事
- 進入低功耗模式
- 不會當機（因為有無限循環）

---

## 為什麼需要 QEMU？

### 開發流程

**沒有 QEMU：**
```
寫程式 → 編譯 → 複製到 SD 卡 → 插到 Pi → 開機 → 測試 → 發現 bug → 重來
（每次測試要 5-10 分鐘）
```

**有 QEMU：**
```
寫程式 → 編譯 → qemu-system-aarch64 → 測試 → 發現 bug → 重來
（每次測試只要 5 秒）
```

### QEMU 的優點
- **快速測試**：不用燒錄 SD 卡
- **除錯方便**：可以看到指令執行過程（`-d in_asm`）
- **安全**：不會燒壞硬體

### QEMU 的限制
- **不是 100% 相同**：某些硬體行為不一樣
- **最終測試**：還是要在真實的 Raspberry Pi 上驗證

---

## Makefile 自動化

手動執行三個命令很麻煩：
```bash
aarch64-linux-gnu-gcc -c a.S -o a.o
aarch64-linux-gnu-ld -T linker_lab0.ld -o kernel8_lab0.elf a.o
aarch64-linux-gnu-objcopy -O binary kernel8_lab0.elf kernel8_lab0.img
```

用 Makefile 一鍵完成：
```bash
make -f Makefile.lab0        # 自動執行上面三個命令
make -f Makefile.lab0 run    # 編譯 + 在 QEMU 執行
make -f Makefile.lab0 clean  # 清理編譯產物
```

---

## Lab 0 的意義

Lab 0 看起來很簡單（只有兩行組合語言），但它教會你：

1. **交叉編譯概念**：在 x86 電腦上編譯 ARM 程式
2. **編譯流程**：源碼 → 目標檔 → ELF → 映像檔
3. **連結器腳本**：控制記憶體佈局
4. **裸機程式設計**：沒有 OS 的環境
5. **硬體規範**：理解 0x80000 這個神奇的數字
6. **開發工具鏈**：gcc, ld, objcopy, qemu

這些都是後續 Lab 的基礎！

---

## 下一步：Lab 1

Lab 0：開機 → 無限休眠  
Lab 1：開機 → 透過 UART 輸出 "Hello World"  
Lab 2：開機 → 互動式 shell  
Lab 3+：記憶體管理、多工、檔案系統...

每個 Lab 都在前一個基礎上加功能，最後你會有一個完整的作業系統！
