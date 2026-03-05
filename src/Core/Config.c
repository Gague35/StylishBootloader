#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include "Config.h"

// Vendor GUID
EFI_GUID gStylishBootGuid = STYLISH_BOOT_GUID;

// ============================================================================
// LOAD TIMEOUT
// ============================================================================

EFI_STATUS LoadTimeout(UINT32* Timeout) {
    UINTN Size = sizeof(UINT32);
    EFI_STATUS Status;
    
    Status = gRT->GetVariable(
        L"StylishBootTimeout",
        &gStylishBootGuid,
        NULL,
        &Size,
        Timeout
    );
    
    if (EFI_ERROR(Status)) {
        // Variable doesn't exist, use default
        *Timeout = DEFAULT_TIMEOUT;
        return EFI_NOT_FOUND;
    }
    
    return EFI_SUCCESS;
}

// ============================================================================
// SAVE TIMEOUT
// ============================================================================

EFI_STATUS SaveTimeout(UINT32 Timeout) {
    return gRT->SetVariable(
        L"StylishBootTimeout",
        &gStylishBootGuid,
        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof(UINT32),
        &Timeout
    );
}

// ============================================================================
// LOAD LAST OS
// ============================================================================

EFI_STATUS LoadLastOS(UINT32* LastOS) {
    UINTN Size = sizeof(UINT32);
    EFI_STATUS Status;
    
    Status = gRT->GetVariable(
        L"StylishBootLastOS",
        &gStylishBootGuid,
        NULL,
        &Size,
        LastOS
    );
    
    if (EFI_ERROR(Status)) {
        // No saved OS, default to first item
        *LastOS = 0;
        return EFI_NOT_FOUND;
    }
    
    return EFI_SUCCESS;
}

// ============================================================================
// SAVE LAST OS
// ============================================================================

EFI_STATUS SaveLastOS(UINT32 LastOS) {
    return gRT->SetVariable(
        L"StylishBootLastOS",
        &gStylishBootGuid,
        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
        sizeof(UINT32),
        &LastOS
    );
}