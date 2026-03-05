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
#include "Config.h"

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
    
    // ========================================================================
    // LOAD CONFIGURATION
    // ========================================================================
    
    UINT32 Timeout = DEFAULT_TIMEOUT;
    UINT32 LastOS = 0;
    
    LoadTimeout(&Timeout);
    LoadLastOS(&LastOS);
    
    // Restore last selected OS (if valid)
    if (LastOS < gOSCount + 2) {  // +2 for Settings and Shutdown
        MenuSetSelected(LastOS);  // ← Définir l'index du carousel
        Print(L"[CONFIG] Restored last OS: %d, Timeout: %d sec\n", LastOS, Timeout);
    } else {
        Print(L"[CONFIG] Invalid LastOS (%d), using default. Timeout: %d sec\n", LastOS, Timeout);
    }
    
    Print(L"Starting menu...\n");
    gBS->Stall(1000000);  // 1 sec
    

    Print(L"Use left/right arrows to naviagte\n");
    Print(L"Press enter to select\n\n");

    // ========================================================================
    // MENU LOOP
    // ========================================================================
    
    BOOLEAN Running = TRUE;
    UINT32 TimerActive = (Timeout > 0) ? 1 : 0;  // Timer enabled?
    UINT32 TimerFrames = Timeout * 60;  // Convert seconds to frames (60 FPS)
    UINT32 CurrentFrame = 0;

    while (Running) {
        INPUT_ACTION Action = PollInput();
        
        // Cancel timer on any input
        if (Action != INPUT_NONE && TimerActive) {
            TimerActive = 0;
            Print(L"[TIMER] Cancelled by user input\n");
        }
        
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
                // Save selected OS before booting
                SaveLastOS(MenuGetSelected());
                
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
        // AUTO-BOOT TIMER
        // ----------------------------------------------------------------
        
        if (TimerActive) {
            CurrentFrame++;
            
            if (CurrentFrame >= TimerFrames) {
                // Timer expired, boot selected OS
                Print(L"[TIMER] Auto-booting OS %d...\n", MenuGetSelected());
                
                // Save selected OS
                SaveLastOS(MenuGetSelected());
                
                Running = FALSE;
                break;
            }
        }

        // ----------------------------------------------------------------
        // SIMPLE RENDER
        // ----------------------------------------------------------------
        
        // Plain background (ultra-fast)
        ClearBackBuffer(RGB(15, 15, 20));  // Dark bluish gray
        
        // Draw carousel
        RenderCarousel(gGraphics.Width, gGraphics.Height);
        
        // ----------------------------------------------------------------
        // DISPLAY TIMER
        // ----------------------------------------------------------------
        
        if (TimerActive) {
            UINT32 SecondsLeft = (TimerFrames - CurrentFrame) / 60 + 1;  // ← Cette ligne DOIT être là !
            
            // Build countdown number (1-2 digits)
            CHAR16 CountdownText[3] = {L'\0', L'\0', L'\0'};
            
            if (SecondsLeft >= 10) {
                // Two digits: "10", "11", etc.
                CountdownText[0] = L'0' + (SecondsLeft / 10);   // Dizaine
                CountdownText[1] = L'0' + (SecondsLeft % 10);   // Unité
            } else {
                // One digit: "9", "8", etc.
                CountdownText[0] = L'0' + SecondsLeft;
            }
            
            DrawStringCenteredScaled(L"Auto-boot in", gGraphics.Width / 2, 50, RGB(200, 200, 200), 1);
            DrawStringCenteredScaled(CountdownText, gGraphics.Width / 2, 80, RGB(255, 100, 100), 3);
            DrawStringCenteredScaled(L"seconds", gGraphics.Width / 2, 130, RGB(200, 200, 200), 1);
        }
        SwapBuffers();
        
        // 60 FPS
        gBS->Stall(16000);
    } 
    
    CleanupFramebuffer();
    
    Print(L"\nMenu termine !\n");
    
    return EFI_SUCCESS;
}