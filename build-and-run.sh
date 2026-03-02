#!/bin/bash
set -e

echo "📦 Creating the UEFI image..."

echo "🔨 Compilation..."
cd ~/uefi-dev/edk2
source edksetup.sh
build -p StylishBootloader/StylishBootloader.dsc -a X64 -t GCC5 -b DEBUG

echo "📦 Creating the BOOT image..."
cd ~/uefi-dev/edk2/StylishBootloader
mkdir -p image/EFI/BOOT
cp ../Build/StylishBootloader/DEBUG_GCC5/X64/StylishBootloader.efi image/EFI/BOOT/BOOTX64.EFI

echo "🚀 QEMU Launch..."
qemu-system-x86_64 \
    -bios /usr/share/edk2/x64/OVMF.4m.fd \
    -drive format=raw,file=fat:rw:image \
    -m 512M \
    -vga std

echo "✅ DONE !"