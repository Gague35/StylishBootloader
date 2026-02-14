#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

// ============================================================================
// TIMER HAUTE PRÉCISION
// ============================================================================

static UINT64 gLastTime = 0;

/**
 * Récupère le temps actuel en microsecondes
 */
UINT64 GetCurrentTimeMicroseconds(VOID) {
    EFI_TIME Time;
    EFI_STATUS Status;
    
    Status = gRT->GetTime(&Time, NULL);
    if (EFI_ERROR(Status)) {
        return 0;
    }
    
    // Convertir en microsecondes
    UINT64 Microseconds = (UINT64)Time.Second * 1000000ULL;
    Microseconds += (UINT64)Time.Nanosecond / 1000ULL;
    
    return Microseconds;
}

/**
 * Initialise le timer
 */
VOID InitializeTimer(VOID) {
    gLastTime = GetCurrentTimeMicroseconds();
}

/**
 * Retourne le delta time en microsecondes
 */
UINT64 GetDeltaTimeMicroseconds(VOID) {
    UINT64 CurrentTime = GetCurrentTimeMicroseconds();
    UINT64 DeltaMicro = CurrentTime - gLastTime;
    gLastTime = CurrentTime;
    
    return DeltaMicro;
}
