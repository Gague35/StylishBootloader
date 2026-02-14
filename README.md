# 🚀 StylishBootloader

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Language: C](https://img.shields.io/badge/Language-C-00599C.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform: UEFI](https://img.shields.io/badge/Platform-UEFI-orange.svg)](https://uefi.org/)
[![Status: In Development](https://img.shields.io/badge/Status-In%20Development-yellow.svg)]()

**A modern UEFI bootloader featuring custom 2D graphics, smooth animations, and an elegant interactive menu.**

---

## 📖 About

**StylishBootloader** is a fully custom UEFI bootloader built from scratch to provide a visually stunning and interactive boot experience. Unlike traditional bootloaders (GRUB, systemd-boot), StylishBootloader focuses on:

- 🎨 **Custom 2D rendering engine** (software-based, no GPU required)
- ✨ **Fluid animations** with time-based interpolation
- 🎯 **Interactive menu** with keyboard navigation and visual feedback
- 🔗 **Multi-OS support** via chainloading (Windows, Linux)
- 🏗️ **Clean modular architecture** for maintainability and extensibility

> **Note**: This is an **educational project** aimed at learning UEFI development, low-level graphics programming, and bootloader internals. It is **not production-ready**.

---

## ✨ Features

### Current Implementation
- ✅ Graphics Output Protocol (GOP) initialization
- ✅ Direct framebuffer access and pixel manipulation
- ✅ Basic primitives (pixel, rectangle, screen clear)
- ✅ Functional UEFI application structure

### Roadmap
- 🚧 Double buffering for flicker-free rendering
- 🚧 Smooth animations (fade, slide, interpolation)
- 🚧 Interactive menu system
- 🚧 Keyboard input handling
- 🚧 GPT partition scanner
- 🚧 OS detection and chainloading
- 🚧 Custom font rendering
- 🚧 Configurable themes

---

## 🏗️ Project Structure

```
StylishBootloader/
├── src/
│   ├── Core/              # System initialization
│   │   ├── Entry.c        # UEFI entry point (UefiMain)
│   │   ├── Platform.c     # GOP setup, timers
│   │   └── Memory.c       # Memory management
│   ├── Graphics/          # 2D rendering engine
│   │   ├── Renderer.c     # Drawing primitives
│   │   ├── Framebuffer.c  # Double buffering
│   │   └── Animations.c   # Interpolation system
│   ├── UI/                # User interface
│   │   ├── Menu.c         # Menu logic
│   │   ├── Transitions.c  # Visual effects
│   │   └── Input.c        # Keyboard handling
│   └── Boot/              # OS booting
│       ├── Scanner.c      # Partition/OS detection
│       └── Chainloader.c  # LoadImage/StartImage
├── docs/
│   ├── ARCHITECTURE.md    # System design
│   ├── BUILD.md           # Build instructions
│   └── UEFI_BASICS.md     # UEFI fundamentals
├── scripts/
│   ├── build.sh           # Automated build
│   └── test-qemu.sh       # QEMU testing
├── StylishBootloader.inf  # EDK2 module manifest
├── StylishBootloader.dsc  # EDK2 platform description
├── .gitignore
├── LICENSE
└── README.md
```

---

## 🛠️ Prerequisites

### Required Software

| Tool | Purpose |
|------|---------|
| **EDK2** | UEFI development framework |
| **GCC 5+** | C compiler (or Clang) |
| **QEMU** | Virtual machine for testing |
| **OVMF** | UEFI firmware for QEMU |
| **Git** | Version control |

### Supported Systems
- ✅ Linux (Ubuntu, Debian, Arch, Fedora, Manjaro)
- ✅ WSL2 (Windows Subsystem for Linux)
- ⚠️ macOS (requires minor adjustments)

---

## 🚀 Quick Start

### 1. Install Dependencies

**Manjaro/Arch:**
```bash
sudo pacman -S base-devel git nasm acpica qemu-system-x86 edk2-ovmf
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential uuid-dev git nasm iasl qemu-system-x86 ovmf
```

**Fedora:**
```bash
sudo dnf install @development-tools libuuid-devel git nasm acpica-tools qemu-system-x86 edk2-ovmf
```

---

### 2. Setup EDK2

```bash
# Clone EDK2 framework
git clone https://github.com/tianocore/edk2.git
cd edk2
git submodule update --init

# Build tools
make -C BaseTools

# Setup environment
source edksetup.sh
```

---

### 3. Clone StylishBootloader

```bash
# Inside edk2 directory
git clone https://github.com/Gague35/StylishBootloader.git
```

---

### 4. Build

```bash
cd edk2

# Configure build target
# Edit Conf/target.txt:
#   ACTIVE_PLATFORM = StylishBootloader/StylishBootloader.dsc
#   TARGET_ARCH = X64
#   TOOL_CHAIN_TAG = GCC5

# Build
build
```

**Output:** `Build/StylishBootloader/DEBUG_GCC5/X64/StylishBootloader.efi`

---

### 5. Test in QEMU

```bash
cd StylishBootloader

# Create boot image
mkdir -p image/EFI/BOOT
cp ../Build/StylishBootloader/DEBUG_GCC5/X64/StylishBootloader.efi \
   image/EFI/BOOT/BOOTX64.EFI

# Launch QEMU
qemu-system-x86_64 \
    -bios /usr/share/ovmf/OVMF.fd \
    -drive format=raw,file=fat:rw:image \
    -m 512M \
    -vga std
```

Or use the automated script:
```bash
./scripts/test-qemu.sh
```

---

## 📚 Documentation

- [**Architecture Overview**](docs/ARCHITECTURE.md) - System design and module responsibilities
- [**Build Guide**](docs/BUILD.md) - Detailed compilation instructions
- [**UEFI Basics**](docs/UEFI_BASICS.md) - Essential UEFI concepts for beginners

---

## 🤝 Contributing

Contributions are welcome! This project is educational, so well-documented and clean code is appreciated.

### How to Contribute

1. **Fork** the repository
2. **Create a feature branch**: `git checkout -b feature/my-feature`
3. **Commit your changes**: `git commit -m 'feat: Add double buffering'`
4. **Push to your fork**: `git push origin feature/my-feature`
5. **Open a Pull Request**

### Coding Guidelines

- **Style**: K&R with 4-space indentation
- **Naming**: `PascalCase` for functions, `UPPER_CASE` for macros
- **Comments**: Clear and educational (this is a learning project)
- **Commits**: Use [Conventional Commits](https://www.conventionalcommits.org/) (`feat:`, `fix:`, `docs:`, etc.)

---

## 🐛 Known Issues & Troubleshooting

| Issue | Solution |
|-------|----------|
| **"Protocol not found"** | Ensure OVMF is installed: `sudo pacman -S edk2-ovmf` |
| **Black screen in QEMU** | Try different VGA modes: `-vga std`, `-vga virtio`, `-vga qxl` |
| **Build fails** | Clean and rebuild BaseTools: `make -C BaseTools clean && make -C BaseTools` |
| **Wrong pixel colors** | UEFI uses BGR format, not RGB (swap R and B values) |

See [BUILD.md](docs/BUILD.md) for detailed troubleshooting.

---

## 📄 License

This project is licensed under the **MIT License** - see [LICENSE](LICENSE) for details.

```
MIT License

Copyright (c) 2025 Gague35

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software...
```

---

## 🙏 Acknowledgments

- [**TianoCore EDK2**](https://github.com/tianocore/edk2) - UEFI development framework
- [**UEFI Specification**](https://uefi.org/specifications) - Official documentation
- [**OSDev Wiki**](https://wiki.osdev.org/) - Invaluable low-level programming resources
- **r/osdev community** - Support and inspiration

---

## 🔗 Useful Resources

### UEFI Documentation
- [UEFI Specification 2.10](https://uefi.org/sites/default/files/resources/UEFI_Spec_2_10_Aug29.pdf)
- [EDK2 Documentation](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II-User-Documentation)
- [GOP Protocol Reference](https://uefi.org/specs/UEFI/2.10/12_Protocols_Console_Support.html#efi-graphics-output-protocol)

### Tutorials
- [OSDev UEFI Guide](https://wiki.osdev.org/UEFI)
- [Bare Metal UEFI Programming](https://krinkinmu.github.io/2020/11/15/efi-getting-started.html)
- [Writing UEFI Applications](https://dvdhrm.github.io/2019/01/31/goodbye-gnuefi/)

### Similar Projects
- [rEFInd Boot Manager](https://www.rodsbooks.com/refind/)
- [systemd-boot](https://www.freedesktop.org/wiki/Software/systemd/systemd-boot/)
- [Limine Bootloader](https://github.com/limine-bootloader/limine)

---

## 📧 Contact

**Maintainer**: Gague35  
**Repository**: [github.com/Gague35/StylishBootloader](https://github.com/Gague35/StylishBootloader)

Found a bug? [Open an issue](https://github.com/Gague35/StylishBootloader/issues)  
Have a question? [Start a discussion](https://github.com/Gague35/StylishBootloader/discussions)

---

<p align="center">
  <sub>Built with ❤️ to learn UEFI, graphics programming, and low-level systems</sub>
</p>

<p align="center">
  <sub>UEFI · C · Graphics · Bootloader</sub>
</p>
