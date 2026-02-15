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
// ANIMATIONS
// ============================================================================

UINT32 LerpUINT32(UINT32 Start, UINT32 End, UINT32 T, UINT32 MaxT);
UINT32 EaseInOutUINT32(UINT32 Start, UINT32 End, UINT32 T, UINT32 MaxT);
UINT32 LerpColor(UINT32 ColorStart, UINT32 ColorEnd, UINT32 T, UINT32 MaxT);
UINT32 CalculateScale(UINT32 Distance, UINT32 MaxDist);
UINT32 CalculateOpacity(UINT32 Distance, UINT32 MaxDist);

// ============================================================================
// RENDERER
// ============================================================================

VOID DrawGlow(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, UINT32 Color, UINT32 Intensity);
VOID DrawRectScaled(UINT32 X, UINT32 Y, UINT32 BaseWidth, UINT32 BaseHeight, UINT32 Scale, UINT32 Color);

// ============================================================================
// MACROS
// ============================================================================

#define RGB(r, g, b) (0xFF000000 | ((r) << 16) | ((g) << 8) | (b))

#endif