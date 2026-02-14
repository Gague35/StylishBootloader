#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>
#include "../Graphics/Graphics.h"

// ============================================================================
// STRUCTURES GLOBALES
// ============================================================================

// `GRAPHICS_CONTEXT` type and `extern gGraphics` are declared in Graphics.h
// Define the single global instance here:
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

    return EFI_SUCCESS;
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
    Print(L"=== Stylish Bootloader v0.3 - Double Buffering ===\n\n");

    Status = InitializeGraphics();
    if (EFI_ERROR(Status)) {
        Print(L"Echec GOP\n");
        return Status;
    }

    Status = InitializeFramebuffer(gGraphics.Width, gGraphics.Height, gGraphics.Framebuffer);
    if (EFI_ERROR(Status)) {
        Print(L"Echec framebuffer\n");
        return Status;
    }

    InitializeTimer();

    Print(L"Animation : Rectangle qui rebondit (DOUBLE BUFFERING)\n");
    Print(L"Appuyez sur une touche pour arreter...\n\n");

    // ========================================================================
    // ANIMATION AVEC DOUBLE BUFFERING
    // ========================================================================

    INT32 PositionX = 0;
    INT32 VelocityX = 3;  // Pixels par frame (simple)
    UINT32 RectWidth = 100;
    UINT32 RectHeight = 100;

    EFI_INPUT_KEY Key;
    UINT32 FrameCount = 0;

    while (TRUE) {
        // Verifier touche
        Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
        if (Status == EFI_SUCCESS) {
            break;
        }

        // Mettre a jour position
        PositionX += VelocityX;

        // Rebondir
        if (PositionX > (INT32)(gGraphics.Width - RectWidth)) {
            PositionX = (INT32)(gGraphics.Width - RectWidth);
            VelocityX = -VelocityX;
        }
        if (PositionX < 0) {
            PositionX = 0;
            VelocityX = -VelocityX;
        }

        // Dessiner dans le BACK BUFFER
        ClearBackBuffer(RGB(0, 0, 0));

        UINT32 X = (UINT32)PositionX;
        UINT32 Y = (gGraphics.Height - RectHeight) / 2;
        DrawFilledRectToBuffer(X, Y, RectWidth, RectHeight, RGB(255, 0, 0));

        // SWAP : Copier vers l'ecran en une fois
        SwapBuffers();

        // Delai pour ~60 FPS
        gBS->Stall(16000);

        FrameCount++;
        if (FrameCount % 60 == 0) {
            Print(L"Frames: %u, PosX: %d\r", FrameCount, PositionX);
        }
    }

    CleanupFramebuffer();

    Print(L"\n\nAnimation terminee ! Total frames: %u\n", FrameCount);

    return EFI_SUCCESS;
}
