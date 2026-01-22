#!/bin/bash
# Lab 0 環境設定腳本

echo "=== Lab 0 環境設定 ==="
echo ""
echo "正在安裝 ARM64 交叉編譯工具鏈和 QEMU..."
echo ""

# 更新套件列表
sudo apt-get update

# 安裝交叉編譯工具鏈
echo "安裝 gcc-aarch64-linux-gnu..."
sudo apt-get install -y gcc-aarch64-linux-gnu

# 安裝 QEMU
echo "安裝 qemu-system-aarch64..."
sudo apt-get install -y qemu-system-aarch64 make

# 驗證安裝
echo ""
echo "=== 驗證安裝 ==="
echo ""

if command -v aarch64-linux-gnu-gcc &> /dev/null; then
    echo "✓ aarch64-linux-gnu-gcc 已安裝"
    aarch64-linux-gnu-gcc --version | head -n 1
else
    echo "✗ aarch64-linux-gnu-gcc 未安裝"
fi

if command -v qemu-system-aarch64 &> /dev/null; then
    echo "✓ qemu-system-aarch64 已安裝"
    qemu-system-aarch64 --version | head -n 1
else
    echo "✗ qemu-system-aarch64 未安裝"
fi

echo ""
echo "=== 設定完成 ==="
echo ""
echo "現在你可以執行："
echo "  make        - 編譯 kernel8.img"
echo "  make run    - 在 QEMU 上執行"
echo "  make clean  - 清理編譯產物"
