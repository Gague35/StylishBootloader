#ifndef OSDETECTOR_H
#define OSDETECTOR_H

#include <Uefi.h>

// OS types
typedef enum {
    OS_TYPE_UNKNOWN = 0,
    OS_TYPE_WINDOWS,
    OS_TYPE_LINUX,
    OS_TYPE_SYSTEMD_BOOT,
    OS_TYPE_UEFI_SHELL
} OS_TYPE;

// Detected OS entry
typedef struct {
    OS_TYPE     Type;               // OS type
    CHAR16      Name[64];           // Display name (e.g., "Windows 11", "Ubuntu 24.04")
    CHAR16      BootPath[256];      // Path to bootloader (e.g., L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi")
    EFI_HANDLE  DiskHandle;         // Disk handle for LoadImage
    BOOLEAN     IsValid;            // Entry is valid
} OS_ENTRY;

// Maximum detected OS
#define MAX_OS_ENTRIES 16

// Global detected OS list
extern OS_ENTRY gDetectedOS[MAX_OS_ENTRIES];
extern UINT32 gOSCount;

// Functions
EFI_STATUS ScanForOperatingSystems(VOID);
VOID PrintDetectedOS(VOID);

#endif