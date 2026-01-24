#!/bin/bash
# Test script for the simple shell

echo "Testing shell commands..."
echo ""

# Create a named pipe for input/output
tmpfile=$(mktemp)

# Send commands and capture output
{
    sleep 0.5
    echo "help"
    sleep 0.5
    echo "hello"
    sleep 0.5
    echo "invalid_command"
    sleep 0.5
} | timeout 8 qemu-system-aarch64 -M raspi3b -kernel kernel8_lab0.img -serial stdio -display none 2>&1 | tee "$tmpfile"

echo ""
echo "Test completed. Output saved to $tmpfile"
