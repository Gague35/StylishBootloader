#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

// ============================================================================
// TIMER
// ============================================================================

static UINT64 gLastTicks = 0;

VOID InitializeTimer(VOID) {
    gLastTicks = 0;
}

UINT64 GetDeltaTimeMicroseconds(VOID) {
    static UINT64 Counter = 0;
    Counter++;

    // Retourner un delta fixe de 16ms (environ 60 FPS)
    return 16000;
}
