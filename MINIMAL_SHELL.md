# Simple Shell - 極簡實作

## 概述
最小化的互動式 Shell，只保留核心功能。

## 功能
- 顯示提示符號 `# `
- 讀取使用者輸入（按 Enter 提交）
- 執行兩個命令：`help` 和 `hello`

## 實作細節

### 程式碼統計
- **總行數**: 136 行
- **二進位大小**: 466 bytes
- **函式數量**: 6 個

### 主要函式

1. **`_start`** - Shell 主迴圈
   - 顯示提示符號
   - 讀取命令
   - 執行命令
   - 重複

2. **`getline`** - 讀取一行輸入
   - 讀取字元直到 Enter (`\n` 或 `\r`)
   - 即時回顯字元
   - Null 終止字串

3. **`run_cmd`** - 執行命令
   - 比對 "help" 和 "hello"
   - 呼叫對應的命令處理函式

4. **`strcmp`** - 字串比較
   - 逐字元比較
   - 相等返回 0

5. **`getc`/`putc`/`puts`** - UART I/O
   - 基本字元和字串輸入輸出

## 移除的功能

與完整版相比，移除了：
- ❌ Backspace 支援
- ❌ 字元過濾（現在接受所有字元）
- ❌ 緩衝區溢位檢查
- ❌ 未知命令錯誤訊息
- ❌ 歡迎橫幅

## 保留的功能

✓ 互動式命令列  
✓ `help` 命令  
✓ `hello` 命令  
✓ 即時字元回顯  
✓ Enter 鍵提交命令  

## 使用方式

### 建置
```bash
make clean && make
```

### 執行
```bash
make run
```

### 範例
```
# help
help: print all available commands
hello: print Hello World!
# hello
Hello World!
# 
```

## 程式碼結構

```
a.S (136 lines)
├── _start          Shell 主迴圈
├── getline         讀取一行
├── run_cmd         執行命令
├── strcmp          字串比較
├── getc            讀取字元
├── putc            輸出字元
└── puts            輸出字串
```

## 記憶體使用

- **.text**: 程式碼
- **.data**: 5 個字串（prompt, messages, command names）
- **.bss**: 128 bytes 命令緩衝區

## 技術特點

### 極簡設計
- 每個函式只做一件事
- 最少的錯誤檢查
- 直接的控制流程

### ARM64 慣例
- 使用 x19-x20 保存值
- x30 儲存返回位址
- x0-x1 傳遞參數

### UART 輪詢
- 簡單的 busy-wait
- 不使用中斷
- 適合裸機環境

## 限制

1. **沒有 Backspace** - 輸入錯誤無法修正
2. **沒有輸入驗證** - 可能接受任何字元
3. **固定緩衝區** - 128 bytes，沒有溢位保護
4. **無錯誤訊息** - 未知命令被忽略

## 適用場景

✓ 學習裸機程式設計  
✓ 理解 Shell 基本原理  
✓ 最小化韌體大小  
✓ 快速原型開發  

## 總結

這是一個**功能完整但極簡**的 Shell：
- 136 行程式碼
- 466 bytes 二進位檔
- 2 個命令
- 無多餘功能
- 容易理解和修改
