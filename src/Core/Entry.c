#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>

// ============================================================================
// STRUCTURES GLOBALES
// ============================================================================

typedef struct {
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    UINT32                       Width;
    UINT32                       Height;
    UINT32                       *Framebuffer;
} GRAPHICS_CONTEXT;

GRAPHICS_CONTEXT gGraphics = {0};

// ============================================================================
// INITIALISATION GOP
// ============================================================================

/**
 * Configure le Graphics Output Protocol et récupère le framebuffer
 * 
 * PIÈGES À ÉVITER :
 * - Le framebuffer est en VRAM physique (write-combining memory)
 * - Format pixel : BGR (pas RGB !) sur la plupart des firmwares
 * - PixelsPerScanLine peut être > Width (padding pour alignement)
 */
EFI_STATUS InitializeGraphics(VOID) {
    EFI_STATUS Status;
    
    // Localiser le GOP (service graphique UEFI)
    Status = gBS->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        (VOID **)&gGraphics.Gop
    );
    
    if (EFI_ERROR(Status)) {
        Print(L"[ERREUR] GOP non disponible : %r\n", Status);
        return Status;
    }
    
    // Récupérer les infos du mode graphique actuel
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info = gGraphics.Gop->Mode->Info;
    gGraphics.Width  = Info->HorizontalResolution;
    gGraphics.Height = Info->VerticalResolution;
    gGraphics.Framebuffer = (UINT32 *)gGraphics.Gop->Mode->FrameBufferBase;
    
    Print(L"[GOP] Resolution : %dx%d\n", gGraphics.Width, gGraphics.Height);
    Print(L"[GOP] Framebuffer : 0x%lx\n", gGraphics.Framebuffer);
    Print(L"[GOP] Format pixel : %d\n", Info->PixelFormat);
    
    // Vérifier le format (on veut BGR ou RGB Reserved)
    if (Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor &&
        Info->PixelFormat != PixelRedGreenBlueReserved8BitPerColor) {
        Print(L"[AVERTISSEMENT] Format pixel non standard !\n");
    }
    
    return EFI_SUCCESS;
}

// ============================================================================
// PRIMITIVES GRAPHIQUES
// ============================================================================

/**
 * Dessine un pixel à (x, y) avec la couleur spécifiée
 * 
 * Format couleur : 0xAARRGGBB (mais stocké en BGR en mémoire)
 * 
 * IMPORTANT : Pas de vérification de limites ici pour la perf.
 * Vérifie avant d'appeler !
 */
VOID DrawPixel(UINT32 X, UINT32 Y, UINT32 Color) {
    if (X >= gGraphics.Width || Y >= gGraphics.Height) {
        return;  // Sécurité
    }
    
    // Calcul de l'offset dans le framebuffer
    UINT32 Offset = Y * gGraphics.Width + X;
    
    // Écriture directe (pas de conversion, le firmware gère BGR)
    gGraphics.Framebuffer[Offset] = Color;
}

/**
 * Remplit un rectangle (non optimisé, version pédagogique)
 */
VOID DrawFilledRect(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, UINT32 Color) {
    for (UINT32 dy = 0; dy < Height; dy++) {
        for (UINT32 dx = 0; dx < Width; dx++) {
            DrawPixel(X + dx, Y + dy, Color);
        }
    }
}

/**
 * Efface l'écran avec une couleur
 */
VOID ClearScreen(UINT32 Color) {
    DrawFilledRect(0, 0, gGraphics.Width, gGraphics.Height, Color);
}

// ============================================================================
// POINT D'ENTRÉE
// ============================================================================

EFI_STATUS EFIAPI UefiMain(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
) {
    EFI_STATUS Status;
    
    // Effacer la console UEFI
    gST->ConOut->ClearScreen(gST->ConOut);
    Print(L"=== Stylish Bootloader v0.1 ===\n\n");
    
    // Initialiser le mode graphique
    Status = InitializeGraphics();
    if (EFI_ERROR(Status)) {
        Print(L"Echec initialisation GOP. Appuyez sur une touche.\n");
        gST->ConIn->Reset(gST->ConIn, FALSE);
        EFI_INPUT_KEY Key;
        while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) != EFI_SUCCESS);
        return Status;
    }
    
    // ========================================================================
    // DEMO : Fond noir + Rectangle rouge central
    // ========================================================================
    
    Print(L"Dessin du fond...\n");
    ClearScreen(0xFF000000);  // Fond noir (format 0xAARRGGBB)
    
    Print(L"Dessin du rectangle...\n");
    // Rectangle rouge au centre
    UINT32 RectWidth = 400;
    UINT32 RectHeight = 300;
    UINT32 RectX = (gGraphics.Width - RectWidth) / 2;
    UINT32 RectY = (gGraphics.Height - RectHeight) / 2;
    DrawFilledRect(RectX, RectY, RectWidth, RectHeight, 0xFF0000FF);  // Rouge en BGR
    
    // Attendre une touche
    Print(L"\n\nAppuyez sur une touche pour quitter...\n");
    gST->ConIn->Reset(gST->ConIn, FALSE);
    EFI_INPUT_KEY Key;
    while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) != EFI_SUCCESS);
    
    return EFI_SUCCESS;
}
