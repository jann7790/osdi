# UART 和 ARM Assembly 筆記

## UART (Universal Asynchronous Receiver Transmitter)

**通用非同步收發傳輸器** - 用於串列通訊的硬件接口

- 樹莓派 3：使用 PL011 UART
- 基地址：0x3F201000
- 應用：輸出調試信息、與主機通訊

## QEMU

**Quick Emulator** - 硬件模擬器

- 可以模擬不同 CPU 架構（ARM、x86、MIPS 等）
- 用途：在開發機上測試為樹莓派設計的 Assembly 代碼，無需實際硬件

## ARM Assembly 寄存器定義

```asm
.equ UART_BASE,   0x3F201000   // 基地址
.equ UART_DR,     0x3F201000   // Data Register（數據寄存器）
.equ UART_FR,     0x3F201018   // Flag Register（狀態寄存器）
```

| 寄存器 | 位置 | 功用 |
|-------|------|------|
| UART_DR | 0x3F201000 | 讀寫實際數據 |
| UART_FR | 0x3F201018 | 檢查 UART 狀態（TXFF、RXFE 等） |

## ARM Assembly 關鍵指令

| 指令 | 功能 |
|------|------|
| `ldr` | Load Register - 從記憶體載入到寄存器 |
| `ldrb` | Load Register Byte - 載入單個字節 |
| `str` | Store Register - 從寄存器寫入到記憶體 |
| `bl` | Branch and Link - 呼叫函數（自動保存返回地址到 x30） |
| `b` | Branch - 無條件跳轉 |
| `ret` | Return - 從 x30 返回 |
| `mov` | Move - 寄存器間複製 |
| `and` | 按位與運算 - 提取特定位 |
| `cbz` | Compare and Branch if Zero - 為零則跳轉 |
| `cbnz` | Compare and Branch if Not Zero - 不為零則跳轉 |
| `wfe` | Wait For Event - 低功耗等待 |

## uart_putc 函數（發送單個字符）

**流程：**
1. 讀取 UART_FR（狀態寄存器）
2. 檢查第 5 位（TXFF）- 發送 FIFO 是否已滿
3. 若滿，忙等待（busy-wait）直到有空間
4. 寫入字符到 UART_DR
5. 硬件自動通過串列發送

## uart_puts 函數（發送整個字符串）

**流程：**
1. 迴圈逐個讀取字符串中的字符
2. 檢查是否為 null 終止符（\0）
3. 若不是，調用 uart_putc 發送字符
4. 重複直到遇到 \0

## UART 通訊的關鍵概念

### FIFO (First In First Out)
- UART 發送 FIFO 容量有限（通常 16 字節）
- 寫入已滿的 FIFO 會導致數據丟失
- **必須先檢查狀態（UART_FR）才能發送**

### 位操作
- `and w2, w2, #0x20`：用來提取特定位（此例為第 5 位）
- 0x20 的二進位是 `0010 0000`，只保留第 5 位

### 寄存器約定
- `x0-x7`：函數參數傳遞、返回值
- `x19-x20`：用於保存臨時數據（在調用其他函數前需保存 x30）
- `x30`：Link Register - 保存函數返回地址（由 `bl` 自動設置）

## 面試重點

1. **UART 是什麼**：硬件接口用於串列通訊
2. **為什麼檢查狀態**：FIFO 有限，防止數據丟失
3. **ARM Assembly 函數調用**：`bl` 保存返回地址到 x30，`ret` 返回
4. **QEMU 的用途**：模擬硬件環境測試代碼
5. **寄存器操作**：`ldr` 讀、`str` 寫、位操作提取信息
