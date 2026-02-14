#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

// ============================================================================
// STRUCTURE GLOBALE GOP
// ============================================================================

typedef struct {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    UINT32                       Width;
    UINT32                       Height;
    UINT32                       *Framebuffer;
} GRAPHICS_CONTEXT;

// Variable globale (définie dans Entry.c)
extern GRAPHICS_CONTEXT gGraphics;

// ============================================================================
// FRAMEBUFFER
// ============================================================================

EFI_STATUS InitializeFramebuffer(UINT32 Width, UINT32 Height, UINT32 *FrontBuffer);
VOID ClearBackBuffer(UINT32 Color);
VOID DrawPixelToBuffer(UINT32 X, UINT32 Y, UINT32 Color);
VOID DrawFilledRectToBuffer(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, UINT32 Color);
VOID SwapBuffers(VOID);
VOID CleanupFramebuffer(VOID);

// ============================================================================
// TIMER
// ============================================================================

VOID InitializeTimer(VOID);
UINT64 GetDeltaTimeMicroseconds(VOID);

// ============================================================================
// MACROS
// ============================================================================

// Convertit RGB en BGR (format UEFI)
#define RGB(r, g, b) (0xFF000000 | ((r) << 16) | ((g) << 8) | (b))

// Animations
UINT32 LerpUINT32(UINT32 Start, UINT32 End, UINT32 T, UINT32 MaxT);
UINT32 EaseInOutUINT32(UINT32 Start, UINT32 End, UINT32 T, UINT32 MaxT);
UINT32 LerpColor(UINT32 ColorStart, UINT32 ColorEnd, UINT32 T, UINT32 MaxT);

#endif