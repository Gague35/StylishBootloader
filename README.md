# 🚀 StylishBootloader

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Language: C](https://img.shields.io/badge/Language-C-00599C.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform: UEFI](https://img.shields.io/badge/Platform-UEFI-orange.svg)](https://uefi.org/)
[![Status: In Development](https://img.shields.io/badge/Status-In%20Development-yellow.svg)]()

A modern UEFI bootloader with custom 2D graphics engine, smooth animations, and an interactive menu system.

---

## ✨ Features

### Current Implementation

- ✅ **Graphics Output Protocol (GOP)** - Direct framebuffer access
- ✅ **Double Buffering** - Flicker-free rendering with swap buffers
- ✅ **Custom 2D Rendering** - Pixel, rectangle, and color primitives
- ✅ **Interactive Menu** - Keyboard navigation with visual feedback
- ✅ **Input Handling** - Arrow keys, Enter, and ESC support
- ✅ **Clean Architecture** - Modular codebase (Core, Graphics, UI, Boot)

### In Progress

- 🚧 **Smooth Transitions** - Fade in/out and slide animations
- 🚧 **Bitmap Font Rendering** - Display text on menu items
- 🚧 **OS Detection** - Scan GPT partitions for bootloaders
- 🚧 **Chainloading** - Boot into Windows, Linux, or other OS

---

## 🎯 Project Goals

This is an **educational project** designed to learn:

- UEFI application development with EDK2
- Low-level graphics programming (software rendering)
- Bootloader architecture and OS booting
- Systems programming in C
- Clean code organization and modularity

> ⚠️ **Note**: This bootloader is **not production-ready**. It's a learning tool and personal project.

---

## 📁 Project Structure
```
StylishBootloader/
├── src/
│   ├── Core/              # System initialization
│   │   ├── Entry.c        # Main entry point (UefiMain)
│   │   └── Platform.c     # Timer and platform services
│   ├── Graphics/          # 2D rendering engine
│   │   ├── Framebuffer.c  # Double buffering implementation
│   │   └── Graphics.h     # Graphics API and macros
│   ├── UI/                # User interface
│   │   ├── Menu.c         # Menu state and rendering
│   │   ├── Input.c        # Keyboard input handling
│   │   └── UI.h           # UI API
│   └── Boot/              # OS detection and chainloading (WIP)
├── StylishBootloader.inf  # EDK2 module manifest
├── StylishBootloader.dsc  # EDK2 platform description
├── build-and-run.sh       # Automated build and test script
├── LICENSE
└── README.md
```

---

## 🛠️ Prerequisites

### Required Software

| Tool | Purpose |
|------|---------|
| **EDK2** | UEFI development framework |
| **GCC 5+** | C compiler |
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
sudo pacman -S base-devel git nasm acpica qemu-system-x86 edk2-ovmf python
```

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential uuid-dev git nasm iasl qemu-system-x86 ovmf python3
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
cd edk2/StylishBootloader

# Automated build and test
./build-and-run.sh
```

**Or manually:**
```bash
cd ~/path/to/edk2
source edksetup.sh
build -p StylishBootloader/StylishBootloader.dsc -a X64 -t GCC5 -b DEBUG

cd StylishBootloader
mkdir -p image/EFI/BOOT
cp ../Build/StylishBootloader/DEBUG_GCC5/X64/StylishBootloader.efi image/EFI/BOOT/BOOTX64.EFI

qemu-system-x86_64 \
    -bios /usr/share/edk2/x64/OVMF.4m.fd \
    -drive format=raw,file=fat:rw:image \
    -m 512M \
    -vga std
```

---

## 🎮 Usage

Once running in QEMU:

- **↑/↓ Arrow Keys** - Navigate menu options
- **Enter** - Select option
- **ESC** - Cancel/Exit

---

## 🏗️ Architecture

### Rendering Pipeline
```
┌─────────────────────────────────────┐
│ 1. Clear Back Buffer (RGB color)   │
│ 2. Draw Menu (rectangles)          │
│ 3. Render UI elements              │
│ 4. SwapBuffers() → Copy to VRAM    │ ← Single atomic operation
└─────────────────────────────────────┘
```

### Double Buffering

- **Back Buffer** - Allocated in RAM (`AllocatePool`)
- **Front Buffer** - Points to GOP framebuffer (VRAM)
- **SwapBuffers()** - Single `CopyMem()` operation (no tearing)

### Color Format

UEFI uses **BGR** (Blue-Green-Red) pixel format:
```c
#define RGB(r, g, b) (0xFF000000 | ((r) << 16) | ((g) << 8) | (b))
```

---

## 🧪 Development

### Build Script

The `build-and-run.sh` script automates:
1. Cleaning old build artifacts
2. Compiling with EDK2
3. Creating bootable image
4. Launching QEMU

### Debugging

Add debug output via:
```c
Print(L"Debug: Value = %d\n", value);
```

Launch QEMU with serial output:
```bash
qemu-system-x86_64 ... -serial stdio
```

---

## 📖 Learning Resources

### UEFI Documentation
- [UEFI Specification 2.10](https://uefi.org/sites/default/files/resources/UEFI_Spec_2_10_Aug29.pdf)
- [EDK2 Documentation](https://github.com/tianocore/tianocore.github.io/wiki/EDK-II-User-Documentation)
- [GOP Protocol](https://uefi.org/specs/UEFI/2.10/12_Protocols_Console_Support.html#efi-graphics-output-protocol)

### Tutorials
- [OSDev UEFI Guide](https://wiki.osdev.org/UEFI)
- [Bare Metal UEFI](https://krinkinmu.github.io/2020/11/15/efi-getting-started.html)

---

## 🗺️ Roadmap

### Phase 1: Graphics Foundation ✅
- [x] GOP initialization
- [x] Pixel/rectangle rendering
- [x] Double buffering
- [x] Color macro (RGB → BGR)

### Phase 2: Interactive UI ✅
- [x] Menu system
- [x] Keyboard input
- [x] Visual selection feedback
- [ ] Smooth transitions
- [ ] Bitmap font rendering

### Phase 3: Boot Functionality 📋
- [ ] GPT partition scanner
- [ ] OS detection (Windows, Linux)
- [ ] Chainloading via LoadImage/StartImage
- [ ] Boot configuration

### Phase 4: Polish 🎨
- [ ] Custom themes
- [ ] Settings menu
- [ ] Error handling
- [ ] Documentation

---

## 🤝 Contributing

This is a personal learning project, but contributions are welcome!

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Commit changes: `git commit -m 'feat: Add my feature'`
4. Push: `git push origin feature/my-feature`
5. Open a Pull Request

### Coding Guidelines

- **Style**: K&R with 4-space indentation
- **Naming**: `PascalCase` for functions, `UPPER_CASE` for macros
- **Comments**: Clear and educational (this is a learning project!)
- **Commits**: Use [Conventional Commits](https://www.conventionalcommits.org/)

---

## 📄 License

This project is licensed under the **MIT License** - see [LICENSE](LICENSE) for details.
```
MIT License

Copyright (c) 2025 Gague35

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...
```

---

## 🙏 Acknowledgments

- [TianoCore EDK2](https://github.com/tianocore/edk2) - UEFI development framework
- [UEFI Forum](https://uefi.org/) - Specifications and standards
- [OSDev Community](https://wiki.osdev.org/) - Invaluable resources
- **Claude (Anthropic)** - Development assistance and guidance

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