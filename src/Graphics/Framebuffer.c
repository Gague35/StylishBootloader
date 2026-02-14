#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

UINT32 *gBackBuffer = NULL;      // Buffer de dessin en mémoire
UINT32 *gFrontBuffer = NULL;     // Pointeur vers VRAM (écran)
UINT32 gBufferWidth = 0;
UINT32 gBufferHeight = 0;
UINTN  gBufferSize = 0;

// ============================================================================
// INITIALISATION DU DOUBLE BUFFERING
// ============================================================================

/**
 * Alloue un back buffer en mémoire
 * 
 * @param Width  Largeur en pixels
 * @param Height Hauteur en pixels
 * @param FrontBuffer Pointeur vers le framebuffer VRAM
 * @return EFI_SUCCESS ou erreur
 */
EFI_STATUS InitializeFramebuffer(UINT32 Width, UINT32 Height, UINT32 *FrontBuffer) {
    gBufferWidth = Width;
    gBufferHeight = Height;
    gFrontBuffer = FrontBuffer;
    gBufferSize = Width * Height * sizeof(UINT32);
    
    // Allouer le back buffer
    gBackBuffer = AllocatePool(gBufferSize);
    
    if (gBackBuffer == NULL) {
        Print(L"[ERREUR] Impossible d'allouer le back buffer (%lu bytes)\n", gBufferSize);
        return EFI_OUT_OF_RESOURCES;
    }
    
    Print(L"[FRAMEBUFFER] Back buffer alloue : %lu KB\n", gBufferSize / 1024);
    
    // Initialiser à noir
    SetMem32(gBackBuffer, gBufferSize, 0xFF000000);
    
    return EFI_SUCCESS;
}

// ============================================================================
// OPÉRATIONS SUR LE BUFFER
// ============================================================================

/**
 * Efface le back buffer avec une couleur
 */
VOID ClearBackBuffer(UINT32 Color) {
    SetMem32(gBackBuffer, gBufferSize, Color);
}

/**
 * Dessine un pixel dans le back buffer
 */
VOID DrawPixelToBuffer(UINT32 X, UINT32 Y, UINT32 Color) {
    if (X >= gBufferWidth || Y >= gBufferHeight) {
        return;
    }
    
    UINT32 Offset = Y * gBufferWidth + X;
    gBackBuffer[Offset] = Color;
}

/**
 * Dessine un rectangle rempli dans le back buffer
 */
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

/**
 * Copie le back buffer vers le front buffer (VRAM)
 * 
 * C'est ici que tout s'affiche à l'écran !
 * Une seule copie = pas de tearing.
 */
VOID SwapBuffers(VOID) {
    if (gBackBuffer == NULL || gFrontBuffer == NULL) {
        return;
    }
    
    // Copier tout le buffer en une fois (rapide !)
    CopyMem(gFrontBuffer, gBackBuffer, gBufferSize);
}

// ============================================================================
// NETTOYAGE
// ============================================================================

/**
 * Libère le back buffer
 */
VOID CleanupFramebuffer(VOID) {
    if (gBackBuffer != NULL) {
        FreePool(gBackBuffer);
        gBackBuffer = NULL;
    }
}
