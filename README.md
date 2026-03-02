# 🚀 StylishBootloader

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Language: C](https://img.shields.io/badge/Language-C-00599C.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Platform: UEFI](https://img.shields.io/badge/Platform-UEFI-orange.svg)](https://uefi.org/)
[![EDK2](https://img.shields.io/badge/Framework-EDK2-blue.svg)](https://github.com/tianocore/edk2)

A modern, minimal UEFI bootloader featuring a smooth animated carousel interface with 60 FPS performance.

---

## ✨ Features

### Implemented

- ✅ **Graphics Output Protocol (GOP)** - Direct framebuffer access (1280×800, 1920×1080, etc.)
- ✅ **Double Buffering** - Flicker-free rendering with atomic buffer swap
- ✅ **Horizontal Carousel UI** - 4-item menu with smooth sliding animations
- ✅ **Advanced Animations** - Ease-in-out cubic transitions (0.33s duration)
- ✅ **Glow Effects** - Multi-layer gradient glow on selected item
- ✅ **Pulse Animation** - Subtle breathing effect (2-second cycle)
- ✅ **Bitmap Font Rendering** - 8×8 font with dynamic scaling
- ✅ **Keyboard Input** - LEFT/RIGHT arrow navigation + Enter/ESC
- ✅ **60 FPS Performance** - Optimized rendering pipeline
- ✅ **Modular Architecture** - Clean separation (Core/Graphics/UI)

### Planned

- 🔜 **OS Detection** - Scan GPT partitions for Windows/Linux bootloaders
- 🔜 **Dynamic Menu** - Auto-generate menu from detected operating systems
- 🔜 **Chainloading** - Boot into selected OS via LoadImage/StartImage
- 🔜 **Settings Menu** - Configure timeout, default OS, resolution
- 🔜 **Theme System** - Customizable colors and animations

---

## 🎯 Design Philosophy

**Performance First**: Stable 60 FPS on all hardware  
**Minimal & Elegant**: No unnecessary features or bloat  
**Educational**: Clean, readable code with learning in mind

---

## 📁 Project Structure
```
StylishBootloader/
├── src/
│   ├── Core/
│   │   ├── Entry.c        # Main entry point & boot loop
│   │   ├── Platform.c     # Timer utilities
│   │   └── Platform.h
│   ├── Graphics/
│   │   ├── Framebuffer.c  # Double buffering (alloc, swap, clear)
│   │   ├── Animations.c   # Interpolation (lerp, ease-in-out)
│   │   ├── Renderer.c     # Primitives (glow, scaled rectangles)
│   │   ├── Font.c         # 8×8 bitmap font (128 chars)
│   │   └── Graphics.h     # Public API & RGB macro
│   └── UI/
│       ├── Menu.c         # Carousel logic & rendering
│       ├── Input.c        # Keyboard polling
│       └── UI.h           # UI types & prototypes
├── StylishBootloader.inf  # EDK2 module manifest
├── StylishBootloader.dsc  # EDK2 build configuration
├── build-and-run.sh       # Build + test automation
├── LICENSE
└── README.md
```

---

## 🎨 UI Showcase

### Carousel Animation
```
    ┌─────┐     ┌───────────┐     ┌─────┐
←   │ Win │     │  LINUX ✨ │     │BIOS │   →
    └─────┘     └───────────┘     └─────┘
     small       selected+glow      small
```

**Features**:
- Center item: Large, red, glowing, pulsing
- Side items: Small, gray, faded
- Smooth slide: Ease-in-out cubic (accelerates then decelerates)
- Scale: 50% (distant) → 100% (center)
- Opacity: 100 (distant) → 255 (center)

### Animation Details

| Property | Value | Notes |
|----------|-------|-------|
| Duration | 20 frames | ~0.33s at 60 FPS |
| Easing | Cubic | 4t³ for smooth acceleration |
| Spacing | 280px | Distance between items |
| Pulse | 0-5% | 2-second breathing cycle |
| Glow | 3 layers | 12/24/36px expansion |

---

## 🛠️ Build Instructions

### Prerequisites

**Required**:
- EDK2 framework (latest stable)
- GCC 5+ or Clang
- NASM assembler
- Python 3
- Git

**For Testing**:
- QEMU (x86_64)
- OVMF firmware

### Installation (Manjaro/Arch)
```bash
sudo pacman -S base-devel git nasm acpica qemu-system-x86 edk2-ovmf python
```

### Installation (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential uuid-dev git nasm iasl \
                 qemu-system-x86 ovmf python3
```

---

### Setup
```bash
# 1. Clone EDK2
git clone https://github.com/tianocore/edk2.git ~/uefi-dev/edk2
cd ~/uefi-dev/edk2
git submodule update --init

# 2. Build EDK2 tools
make -C BaseTools

# 3. Setup environment
source edksetup.sh

# 4. Clone StylishBootloader
git clone https://github.com/Gague35/StylishBootloader.git

# 5. Build & run
cd StylishBootloader
./build-and-run.sh
```

---

### Manual Build
```bash
cd ~/uefi-dev/edk2
source edksetup.sh
build -p StylishBootloader/StylishBootloader.dsc -a X64 -t GCC5 -b DEBUG

cd StylishBootloader
mkdir -p image/EFI/BOOT
cp ../Build/StylishBootloader/DEBUG_GCC5/X64/StylishBootloader.efi \
   image/EFI/BOOT/BOOTX64.EFI

qemu-system-x86_64 \
    -bios /usr/share/edk2/x64/OVMF.4m.fd \
    -drive format=raw,file=fat:rw:image \
    -m 512M
```

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| **←** | Move left (wraps around) |
| **→** | Move right (wraps around) |
| **Enter** | Select option |
| **ESC** | Exit bootloader |

> **Note**: ↑/↓ arrows are disabled (carousel is horizontal only)

---

## 🏗️ Technical Architecture

### Rendering Pipeline
```
┌─────────────────────────────────────────┐
│ Input Polling                           │ ← 16ms budget (60 FPS)
├─────────────────────────────────────────┤
│ 1. MenuUpdate()                         │ ← Animate offset
│    - Interpolate AnimationOffset        │
│    - EaseInOutUINT32 (cubic)            │
├─────────────────────────────────────────┤
│ 2. ClearBackBuffer(RGB(15,15,20))      │ ← Solid color fill
├─────────────────────────────────────────┤
│ 3. RenderCarousel()                     │
│    For each item:                       │
│      - Calculate position & distance    │
│      - Compute scale & opacity          │
│      - Draw glow (3 layers)             │
│      - Draw rectangle                   │
│      - Draw text (scaled font)          │
├─────────────────────────────────────────┤
│ 4. SwapBuffers()                        │ ← CopyMem to VRAM
├─────────────────────────────────────────┤
│ 5. Stall(16000μs)                       │ ← Frame limiter
└─────────────────────────────────────────┘
```

### Double Buffering
```c
gBackBuffer  (RAM)  → Draw here all frame
       ↓
SwapBuffers() → CopyMem()
       ↓
gFrontBuffer (VRAM) → Display on screen
```

**Why?**
- Prevents tearing (no partial frames visible)
- Atomic swap (single memcpy operation)
- Clean separation: draw vs display

---

## 🎨 Animation System

### Ease-In-Out Cubic
```c
f(t) = 4t³           for t ∈ [0, 0.5]   // Accelerate
f(t) = 1 - 4(1-t)³   for t ∈ [0.5, 1]   // Decelerate
```

**Effect**: Smooth start → fast middle → smooth stop

### Scale Calculation
```c
Scale = 100% - (Distance × 50%) / MaxDistance

Distance 0   → 100% (full size)
Distance 500 → 50%  (half size)
```

### Opacity Calculation
```c
Opacity = 255 - (Distance × 155) / MaxDistance

Distance 0   → 255 (opaque)
Distance 500 → 100 (translucent)
```

---

## 🔤 Font System

### Bitmap Format

- **Size**: 8×8 pixels per character
- **Coverage**: 128 ASCII characters (0x00-0x7F)
- **Storage**: `static const UINT8 font8x8_basic[128][8]`
- **Scaling**: 1×, 2×, 3× (integer scaling only)

### Rendering
```c
DrawChar(ch, x, y, color)           // 8×8 pixels
DrawCharScaled(ch, x, y, color, 2)  // 16×16 pixels
DrawString("Text", x, y, color)     // Auto-spacing
DrawStringCentered("Text", cx, y)   // Auto-center
```

**Example**:
```
'A' (0x41) bitmap:
0x18 → 00011000
0x24 → 00100100
0x42 → 01000010
0x7E → 01111110
0x42 → 01000010
0x42 → 01000010
0x00 → 00000000
```

---

## 📊 Performance

### Target: 60 FPS (16.6ms per frame)

| Operation | Time | Notes |
|-----------|------|-------|
| Input polling | <0.1ms | Non-blocking |
| MenuUpdate | <0.1ms | Integer math only |
| ClearBackBuffer | 1-2ms | SetMem32 (1280×800) |
| RenderCarousel | 2-4ms | 4 items × (glow + rect + text) |
| SwapBuffers | 2-3ms | CopyMem (3.9 MB @ 1920×1080) |
| **Total** | **~8ms** | **50% margin for 60 FPS** |

### Memory Usage

- Back buffer: 3.9 MB (1920×1080×4 bytes)
- Font data: 1 KB (128 × 8 bytes)
- State: <100 bytes (CAROUSEL_STATE)
- **Total**: ~4 MB

---

## 🐛 Known Issues

### Input

- ↑/↓ arrows disabled (not needed for horizontal carousel)
- QEMU may have unusual key mappings (tested and corrected)

### Graphics

- No multi-monitor support
- Fixed to native GOP resolution (no scaling)
- Background images removed (caused FPS drops)

### Future Limitations

- No UTF-8 support (ASCII only, 0x00-0x7F)
- No anti-aliased fonts (bitmap only)
- No GPU acceleration (software rendering)

---

## 🗺️ Roadmap

### ✅ Phase 1: Graphics Foundation (Complete)
- [x] GOP initialization
- [x] Double buffering
- [x] Pixel/rectangle rendering
- [x] Color format (BGR correction)

### ✅ Phase 2: Carousel UI (Complete)
- [x] Horizontal menu system
- [x] Keyboard input (LEFT/RIGHT)
- [x] Ease-in-out animations
- [x] Glow + pulse effects
- [x] Bitmap font rendering

### 🔜 Phase 3: Boot Functionality (Next)
- [ ] GPT partition scanner
- [ ] EFI bootloader detection (bootmgfw.efi, grubx64.efi)
- [ ] OS identification (Windows, Linux distros)
- [ ] Dynamic menu generation
- [ ] Chainloading via LoadImage/StartImage

### 🔜 Phase 4: Polish & Settings
- [ ] Settings menu (timeout, default OS)
- [ ] Resolution selection
- [ ] Custom color schemes
- [ ] Boot configuration persistence
- [ ] Error handling & recovery

---

## 🤝 Contributing

This is a personal learning project, but contributions are welcome!

### How to Contribute

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/amazing-feature`
3. Commit: `git commit -m 'feat: Add amazing feature'`
4. Push: `git push origin feature/amazing-feature`
5. Open a Pull Request

### Coding Standards

- **Language**: C99 (no C++ features)
- **Style**: 4-space indent, K&R braces
- **Naming**: 
  - Functions: `PascalCase` (e.g., `DrawPixel`)
  - Variables: `camelCase` (e.g., `screenWidth`)
  - Macros: `UPPER_CASE` (e.g., `RGB`)
  - Globals: `g` prefix (e.g., `gGraphics`)
- **Comments**: English, concise, explain "why" not "what"
- **Commits**: [Conventional Commits](https://www.conventionalcommits.org/) format

---

## 📚 Learning Resources

### UEFI Specifications
- [UEFI 2.10 Specification](https://uefi.org/specs/UEFI/2.10/)
- [PI 1.8 Specification](https://uefi.org/specs/PI/1.8/)
- [GOP Protocol](https://uefi.org/specs/UEFI/2.10/12_Protocols_Console_Support.html#efi-graphics-output-protocol)

### EDK2 Documentation
- [EDK2 Getting Started](https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II)
- [EDK2 Build](https://github.com/tianocore/tianocore.github.io/wiki/Common-instructions)
- [UEFI Driver Writer's Guide](https://github.com/tianocore-docs/edk2-UefiDriverWritersGuide)

### Community Resources
- [OSDev UEFI](https://wiki.osdev.org/UEFI)
- [r/osdev](https://reddit.com/r/osdev)
- [Bare Metal UEFI Guide](https://krinkinmu.github.io/2020/11/15/efi-getting-started.html)

---

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.
```
Copyright (c) 2026 Gague35

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software...
```

---

## 🙏 Acknowledgments

- **TianoCore EDK2** - UEFI development framework
- **UEFI Forum** - Technical specifications
- **OSDev Community** - Educational resources
- **Claude (Anthropic)** - Development assistance

---

## 📧 Contact

**Author**: Gague35  
**GitHub**: [github.com/Gague35/StylishBootloader](https://github.com/Gague35/StylishBootloader)  
**Issues**: [Report a bug](https://github.com/Gague35/StylishBootloader/issues)

---

<p align="center">
  <sub>Built to learn UEFI, graphics, and low-level systems programming</sub>
</p>

<p align="center">
  <sub>🚀 Fast · ✨ Minimal · 🎨 Smooth</sub>
</p>