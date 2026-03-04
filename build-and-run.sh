#!/bin/bash
set -e

echo "📦 Creating the UEFI image..."
cd ~/uefi-dev/edk2/StylishBootloader
rm -rf image/

echo "🔨 Compilation..."
cd ~/uefi-dev/edk2
source edksetup.sh
build -p StylishBootloader/StylishBootloader.dsc -a X64 -t GCC5 -b DEBUG

echo "📦 Creating the BOOT image..."
cd ~/uefi-dev/edk2/StylishBootloader
mkdir -p image/EFI/BOOT
cp ../Build/StylishBootloader/DEBUG_GCC5/X64/StylishBootloader.efi image/EFI/BOOT/BOOTX64.EFI

# Create fake bootloaders for testing OS detection
echo "🧪 Creating test OS structure..."

# Windows
mkdir -p image/EFI/Microsoft/Boot
cp image/EFI/BOOT/BOOTX64.EFI image/EFI/Microsoft/Boot/bootmgfw.efi

# Ubuntu
mkdir -p image/EFI/ubuntu
cp image/EFI/BOOT/BOOTX64.EFI image/EFI/ubuntu/grubx64.efi

# Arch Linux
mkdir -p image/EFI/arch
cp image/EFI/BOOT/BOOTX64.EFI image/EFI/arch/grubx64.efi

# Fedora
mkdir -p image/EFI/fedora
cp image/EFI/BOOT/BOOTX64.EFI image/EFI/fedora/grubx64.efi

echo "✅ Test OS structure created"

echo "🚀 QEMU Launch..."
qemu-system-x86_64 \
    -bios /usr/share/edk2/x64/OVMF.4m.fd \
    -drive format=raw,file=fat:rw:image \
    -m 512M \
    -vga std

echo "✅ DONE !"