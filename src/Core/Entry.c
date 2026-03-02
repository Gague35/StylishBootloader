#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h> 
#include <Protocol/GraphicsOutput.h>
#include "../Graphics/Graphics.h"
#include "../UI/UI.h"
#include "Platform.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

GRAPHICS_CONTEXT gGraphics = {0};

// ============================================================================
// GOP INITIATION
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
// ENTRY POINT
// ============================================================================

EFI_STATUS EFIAPI UefiMain(
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
) {
    EFI_STATUS Status;
    
    gST->ConOut->ClearScreen(gST->ConOut);
    Print(L"=== Stylish Bootloader v0.4 - Interactive Menu ===\n\n");
    
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
    

    Print(L"Use left/right arrows to naviagte\n");
    Print(L"Press enter to select\n\n");

    // ========================================================================
    // MENU LOOP
    // ========================================================================
    
    BOOLEAN Running = TRUE;

    while (Running) {
        INPUT_ACTION Action = PollInput();
        
        switch (Action) {
            case INPUT_LEFT:
                MenuMoveLeft();
                break;
            
            case INPUT_RIGHT:
                MenuMoveRight();
                break;
            
            case INPUT_SELECT:
                Print(L"Option selectionnee : %u\n", MenuGetSelected());
                Running = FALSE;
                break;
            
            case INPUT_ESCAPE:
                Print(L"Annule\n");
                Running = FALSE;
                break;
            
            default:
                break;
        }

        MenuUpdate();

        // ----------------------------------------------------------------
        // SIMPLE RENDER
        // ----------------------------------------------------------------
        
        // Plain background (ultra-fast)
        ClearBackBuffer(RGB(15, 15, 20));  // Dark bluish gray
        
        // Draw carousel
        RenderCarousel(gGraphics.Width, gGraphics.Height);
        
        // Swap buffers
        SwapBuffers();
        
        // 60 FPS
        gBS->Stall(16000);
    } 
    
    CleanupFramebuffer();
    
    Print(L"\nMenu termine !\n");
    
    return EFI_SUCCESS;
}