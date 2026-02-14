#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>
#include "../Graphics/Graphics.h"
#include "../UI/UI.h"

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// Variable globale (définie ici, déclarée dans Graphics.h)
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
    
    Print(L"Menu interactif : Utilisez les fleches haut/bas\n");
    Print(L"Appuyez sur Entree pour selectionner\n\n");
    
    // ========================================================================
    // BOUCLE DU MENU
    // ========================================================================
    
    BOOLEAN Running = TRUE;
    
    while (Running) {
        // Gérer l'input
        INPUT_ACTION Action = PollInput();
        
        switch (Action) {
            case INPUT_UP:
                MenuMoveUp();
                break;
            
            case INPUT_DOWN:
                MenuMoveDown();
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
        
        // Dessiner le menu
        ClearBackBuffer(RGB(20, 20, 30));  // Fond bleu très foncé
        RenderMenu(gGraphics.Width, gGraphics.Height);
        SwapBuffers();
        
        // Petit délai pour éviter de spammer le CPU
        gBS->Stall(16000);  // 16ms
    }
    
    CleanupFramebuffer();
    
    Print(L"\nMenu termine !\n");
    
    return EFI_SUCCESS;
}