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

EFI_STATUS InitializeGraphics(VOID) {
    EFI_STATUS Status;

    Status = gBS->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        (VOID **)&gGraphics.Gop
    );

    if (EFI_ERROR(Status)) {
        Print(L"[ERREUR] GOP non disponible : %r\n", Status);
        return Status;
    }

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info = gGraphics.Gop->Mode->Info;
    gGraphics.Width  = Info->HorizontalResolution;
    gGraphics.Height = Info->VerticalResolution;
    gGraphics.Framebuffer = (UINT32 *)gGraphics.Gop->Mode->FrameBufferBase;

    Print(L"[GOP] Resolution : %dx%d\n", gGraphics.Width, gGraphics.Height);
    Print(L"[GOP] Framebuffer : 0x%lx\n", gGraphics.Framebuffer);

    return EFI_SUCCESS;
}

// ============================================================================
// DESSIN DIRECT (pas de buffer)
// ============================================================================

VOID DrawRectDirect(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, UINT32 Color) {
    for (UINT32 dy = 0; dy < Height; dy++) {
        for (UINT32 dx = 0; dx < Width; dx++) {
            UINT32 px = X + dx;
            UINT32 py = Y + dy;

            if (px < gGraphics.Width && py < gGraphics.Height) {
                UINT32 Offset = py * gGraphics.Width + px;
                gGraphics.Framebuffer[Offset] = Color;
            }
        }
    }
}

VOID ClearScreenDirect(UINT32 Color) {
    for (UINT32 i = 0; i < gGraphics.Width * gGraphics.Height; i++) {
        gGraphics.Framebuffer[i] = Color;
    }
}

// ============================================================================
// POINT D'ENTREE
// ============================================================================

EFI_STATUS EFIAPI UefiMain(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
) {
    EFI_STATUS Status;

    gST->ConOut->ClearScreen(gST->ConOut);
    Print(L"=== TEST DIRECT (sans buffer) ===\n\n");

    Status = InitializeGraphics();
    if (EFI_ERROR(Status)) {
        Print(L"Echec GOP\n");
        return Status;
    }

    Print(L"Dessin direct dans VRAM...\n");
    Print(L"Le rectangle DOIT bouger maintenant !\n\n");

    UINT32 RectWidth = 100;
    UINT32 RectHeight = 100;
    UINT32 Y = (gGraphics.Height - RectHeight) / 2;

    // Tester 3 couleurs
    for (UINT32 TestX = 0; TestX < gGraphics.Width - RectWidth; TestX += 5) {
        // Effacer l'ecran (noir)
        ClearScreenDirect(0xFF000000);

        // Dessiner le rectangle
        UINT32 Color;
        if (TestX < 200) {
            Color = 0xFF0000FF;  // Rouge (B=FF, G=00, R=00)
            Print(L"Phase 1 : Rouge\r");
        } else if (TestX < 400) {
            Color = 0xFF00FF00;  // Vert (B=00, G=FF, R=00)
            Print(L"Phase 2 : Vert \r");
        } else {
            Color = 0xFFFF0000;  // Bleu (B=00, G=00, R=FF)
            Print(L"Phase 3 : Bleu \r");
        }

        DrawRectDirect(TestX, Y, RectWidth, RectHeight, Color);

        gBS->Stall(10000);  // 10ms

        // Verifier touche
        EFI_INPUT_KEY Key;
        Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        if (Status == EFI_SUCCESS) {
            break;
        }
    }

    Print(L"\n\nTest termine !\n");
    Print(L"Appuyez sur une touche...\n");

    gST->ConIn->Reset(gST->ConIn, FALSE);
    EFI_INPUT_KEY Key;
    while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) != EFI_SUCCESS);

    return EFI_SUCCESS;
}
