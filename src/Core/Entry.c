#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h> 
#include <Protocol/GraphicsOutput.h>
#include "../Graphics/Graphics.h"
#include "../UI/UI.h"
#include "Platform.h"
#include "../Boot/OSDetector.h"

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
    
    // ======== FORCER LA MEILLEURE RÉSOLUTION 16:9 ========
    UINT32 BestMode = 0;
    UINT32 BestWidth = gGraphics.Width;
    UINT32 BestHeight = gGraphics.Height;
    
    for (UINT32 i = 0; i < gGraphics.Gop->Mode->MaxMode; i++) {
        EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
        UINTN SizeOfInfo;
        
        Status = gGraphics.Gop->QueryMode(gGraphics.Gop, i, &SizeOfInfo, &Info);
        if (!EFI_ERROR(Status)) {
            UINT32 W = Info->HorizontalResolution;
            UINT32 H = Info->VerticalResolution;
            
            // Check 16:9 ratio
            if ((W * 9) == (H * 16)) {
                if (W > BestWidth) {
                    BestMode = i;
                    BestWidth = W;
                    BestHeight = H;
                }
            }
        }
    }
    
    // Apply best mode if found
    if (BestWidth > gGraphics.Width) {
        Print(L"[GOP] Switching to %dx%d...\n", BestWidth, BestHeight);
        Status = gGraphics.Gop->SetMode(gGraphics.Gop, BestMode);
        if (!EFI_ERROR(Status)) {
            gGraphics.Width = BestWidth;
            gGraphics.Height = BestHeight;
            gGraphics.Framebuffer = (UINT32 *)gGraphics.Gop->Mode->FrameBufferBase;
            Print(L"[GOP] Success! Resolution: %dx%d\n", gGraphics.Width, gGraphics.Height);
        }
    } else {
        Print(L"[GOP] Using default: %dx%d\n", gGraphics.Width, gGraphics.Height);
    }
    
    gBS->Stall(1000000);  // 1 sec
    
    Status = InitializeFramebuffer(gGraphics.Width, gGraphics.Height, gGraphics.Framebuffer);
    if (EFI_ERROR(Status)) {
        Print(L"Echec framebuffer\n");
        return Status;
    }

    // Detect operating systems
    Status = ScanForOperatingSystems();
    if (EFI_ERROR(Status)) {
        Print(L"[WARN] OS detection failed: %r\n", Status);
        gBS->Stall(1000000);  // 1 sec pour voir l'erreur
    }
    
    Print(L"Starting menu...\n");
    gBS->Stall(500000);  // 0.5 sec
    

    Print(L"Use left/right arrows to naviagte\n");
    Print(L"Press enter to select\n\n");

    // ========================================================================
    // MENU LOOP
    // ========================================================================
    
    BOOLEAN Running = TRUE;

    while (Running) {
        INPUT_ACTION Action = PollInput();
        
        switch (Action) {
            case INPUT_UP:
                MenuMoveUp();
                break;
            
            case INPUT_DOWN:
                MenuMoveDown();
                break;
            
            case INPUT_LEFT:
                MenuMoveLeft();
                break;
            
            case INPUT_RIGHT:
                MenuMoveRight();
                break;
            
            case INPUT_SELECT:
                Print(L"Option selected : %u\n", MenuGetSelected());
                Running = FALSE;
                break;
            
            case INPUT_ESCAPE:
                Print(L"Cancel\n");
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