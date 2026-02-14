#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

UINT32 *gBackBuffer = NULL;
UINT32 *gFrontBuffer = NULL;
UINT32 gBufferWidth = 0;
UINT32 gBufferHeight = 0;
UINTN  gBufferSize = 0;

// ============================================================================
// INITIALISATION
// ============================================================================

EFI_STATUS InitializeFramebuffer(UINT32 Width, UINT32 Height, UINT32 *FrontBuffer) {
    gBufferWidth = Width;
    gBufferHeight = Height;
    gFrontBuffer = FrontBuffer;
    gBufferSize = Width * Height * sizeof(UINT32);

    gBackBuffer = AllocatePool(gBufferSize);

    if (gBackBuffer == NULL) {
        Print(L"[ERREUR] Impossible d'allouer le back buffer (%lu bytes)\n", gBufferSize);
        return EFI_OUT_OF_RESOURCES;
    }

    Print(L"[FRAMEBUFFER] Back buffer alloue : %lu KB\n", gBufferSize / 1024);

    SetMem32(gBackBuffer, gBufferSize, 0xFF000000);

    return EFI_SUCCESS;
}

// ============================================================================
// OPERATIONS
// ============================================================================

VOID ClearBackBuffer(UINT32 Color) {
    SetMem32(gBackBuffer, gBufferSize, Color);
}

VOID DrawPixelToBuffer(UINT32 X, UINT32 Y, UINT32 Color) {
    if (X >= gBufferWidth || Y >= gBufferHeight) {
        return;
    }

    UINT32 Offset = Y * gBufferWidth + X;
    gBackBuffer[Offset] = Color;
}

VOID DrawFilledRectToBuffer(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, UINT32 Color) {
    for (UINT32 dy = 0; dy < Height; dy++) {
        for (UINT32 dx = 0; dx < Width; dx++) {
            UINT32 px = X + dx;
            UINT32 py = Y + dy;

            if (px < gBufferWidth && py < gBufferHeight) {
                DrawPixelToBuffer(px, py, Color);
            }
        }
    }
}

// ============================================================================
// SWAP BUFFERS
// ============================================================================

VOID SwapBuffers(VOID) {
    if (gBackBuffer == NULL || gFrontBuffer == NULL) {
        return;
    }

    CopyMem(gFrontBuffer, gBackBuffer, gBufferSize);
}

// ============================================================================
// NETTOYAGE
// ============================================================================

VOID CleanupFramebuffer(VOID) {
    if (gBackBuffer != NULL) {
        FreePool(gBackBuffer);
        gBackBuffer = NULL;
    }
}
