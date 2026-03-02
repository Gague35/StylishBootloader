#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

// ============================================================================
// INPUT HANDLING
// ============================================================================

typedef enum {
    INPUT_NONE,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_SELECT,
    INPUT_ESCAPE
} INPUT_ACTION;

INPUT_ACTION PollInput(VOID) {
    EFI_INPUT_KEY Key;
    EFI_STATUS Status;
    
    Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
    
    if (EFI_ERROR(Status)) {
        return INPUT_NONE;
    }
    
    // Normal keys
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {  // ENTRY
        return INPUT_SELECT;
    }
    
    // Special keys (arrows)
    if (Key.ScanCode == SCAN_ESC) {
        return INPUT_ESCAPE;
    }
    if (Key.ScanCode == SCAN_LEFT) {
        return INPUT_LEFT;
    }
    if (Key.ScanCode == SCAN_RIGHT) {
        return INPUT_RIGHT;
    }   
    
    return INPUT_NONE;
}