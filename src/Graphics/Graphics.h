#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Uefi.h>

// Framebuffer
EFI_STATUS InitializeFramebuffer(UINT32 Width, UINT32 Height, UINT32 *FrontBuffer);
VOID ClearBackBuffer(UINT32 Color);
VOID DrawPixelToBuffer(UINT32 X, UINT32 Y, UINT32 Color);
VOID DrawFilledRectToBuffer(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, UINT32 Color);
VOID SwapBuffers(VOID);
VOID CleanupFramebuffer(VOID);

// Graphics context shared across modules
typedef struct {
	EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
	UINT32                       Width;
	UINT32                       Height;
	UINT32                       *Framebuffer;
} GRAPHICS_CONTEXT;

// Global instance (defined in one C file)
extern GRAPHICS_CONTEXT gGraphics;

// Timer - Retourne microsecondes écoulées depuis dernier appel
VOID InitializeTimer(VOID);
UINT64 GetDeltaTimeMicroseconds(VOID);

// Macro RGB -> BGR (UEFI framebuffer expects BGR ordering)
// Swap R and B so RGB(r,g,b) produces 0xAABBGGRR in memory when copied to VRAM
#define RGB(r, g, b) (0xFF000000 | ((b) << 16) | ((g) << 8) | (r))

#endif
