#ifndef CONFIG_H
#define CONFIG_H

#include <Uefi.h>

// Vendor GUID for StylishBootloader variables
#define STYLISH_BOOT_GUID \
    { 0x12345678, 0x1234, 0x5678, { 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34, 0x56, 0x78 } }

// Default configuration
#define DEFAULT_TIMEOUT 10  // 10 seconds

// Load/Save functions
EFI_STATUS LoadTimeout(UINT32* Timeout);
EFI_STATUS SaveTimeout(UINT32 Timeout);
EFI_STATUS LoadLastOS(UINT32* LastOS);
EFI_STATUS SaveLastOS(UINT32 LastOS);

#endif