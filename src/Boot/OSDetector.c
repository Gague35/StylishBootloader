#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/BlockIo.h>
#include <Guid/FileInfo.h>
#include "OSDetector.h"

// ============================================================================
// GLOBAL DATA
// ============================================================================

OS_ENTRY gDetectedOS[MAX_OS_ENTRIES];
UINT32 gOSCount = 0;

// ============================================================================
// HELPER: Check if file exists
// ============================================================================

BOOLEAN FileExists(EFI_FILE_PROTOCOL* Root, CHAR16* Path) {
    EFI_STATUS Status;
    EFI_FILE_PROTOCOL* File;
    
    Status = Root->Open(Root, &File, Path, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(Status)) {
        return FALSE;
    }
    
    File->Close(File);
    return TRUE;
}

// ============================================================================
// HELPER: Add OS to list
// ============================================================================

VOID AddOS(OS_TYPE Type, CHAR16* Name, CHAR16* BootPath, EFI_HANDLE DiskHandle) {
    if (gOSCount >= MAX_OS_ENTRIES) {
        Print(L"[WARN] Max OS entries reached (%d)\n", MAX_OS_ENTRIES);
        return;
    }
    
    OS_ENTRY* entry = &gDetectedOS[gOSCount];
    entry->Type = Type;
    entry->DiskHandle = DiskHandle;
    entry->IsValid = TRUE;
    
    // Copy strings
    UINTN i;
    for (i = 0; i < 63 && Name[i] != L'\0'; i++) {
        entry->Name[i] = Name[i];
    }
    entry->Name[i] = L'\0';
    
    for (i = 0; i < 255 && BootPath[i] != L'\0'; i++) {
        entry->BootPath[i] = BootPath[i];
    }
    entry->BootPath[i] = L'\0';
    
    gOSCount++;
    Print(L"[OS] Found: %s (%s)\n", Name, BootPath);
}

// ============================================================================
// SCAN: Detect OS on a single ESP partition
// ============================================================================

EFI_STATUS ScanESP(EFI_HANDLE Handle, EFI_FILE_PROTOCOL* Root) {
    // Check for Windows
    if (FileExists(Root, L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi")) {
        AddOS(OS_TYPE_WINDOWS, L"Windows", L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi", Handle);
    }
    
    // Check for Ubuntu
    if (FileExists(Root, L"\\EFI\\ubuntu\\grubx64.efi")) {
        AddOS(OS_TYPE_LINUX, L"Ubuntu", L"\\EFI\\ubuntu\\grubx64.efi", Handle);
    }
    
    // Check for Fedora
    if (FileExists(Root, L"\\EFI\\fedora\\grubx64.efi")) {
        AddOS(OS_TYPE_LINUX, L"Fedora", L"\\EFI\\fedora\\grubx64.efi", Handle);
    }
    
    // Check for Arch
    if (FileExists(Root, L"\\EFI\\arch\\grubx64.efi")) {
        AddOS(OS_TYPE_LINUX, L"Arch Linux", L"\\EFI\\arch\\grubx64.efi", Handle);
    }
    
    // Check for Debian
    if (FileExists(Root, L"\\EFI\\debian\\grubx64.efi")) {
        AddOS(OS_TYPE_LINUX, L"Debian", L"\\EFI\\debian\\grubx64.efi", Handle);
    }
    
    // Check for systemd-boot
    if (FileExists(Root, L"\\EFI\\systemd\\systemd-bootx64.efi")) {
        AddOS(OS_TYPE_SYSTEMD_BOOT, L"systemd-boot", L"\\EFI\\systemd\\systemd-bootx64.efi", Handle);
    }
    
    // Check for UEFI Shell
    if (FileExists(Root, L"\\EFI\\tools\\Shell.efi")) {
        AddOS(OS_TYPE_UEFI_SHELL, L"UEFI Shell", L"\\EFI\\tools\\Shell.efi", Handle);
    }
    
    return EFI_SUCCESS;
}

// ============================================================================
// MAIN SCAN
// ============================================================================

EFI_STATUS ScanForOperatingSystems(VOID) {
    EFI_STATUS Status;
    UINTN HandleCount = 0;
    EFI_HANDLE* Handles = NULL;
    
    Print(L"\n[OS Detection] Scanning for operating systems...\n");
    
    // Reset list
    gOSCount = 0;
    
    // Locate all SimpleFileSystem handles (ESP partitions)
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &HandleCount,
        &Handles
    );
    
    if (EFI_ERROR(Status)) {
        Print(L"[ERROR] Failed to locate file systems: %r\n", Status);
        return Status;
    }
    
    Print(L"[INFO] Found %d file system(s)\n", HandleCount);
    
    // Scan each filesystem
    for (UINTN i = 0; i < HandleCount; i++) {
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* Fs;
        EFI_FILE_PROTOCOL* Root;
        
        Status = gBS->HandleProtocol(Handles[i], &gEfiSimpleFileSystemProtocolGuid, (VOID**)&Fs);
        if (EFI_ERROR(Status)) {
            continue;
        }
        
        Status = Fs->OpenVolume(Fs, &Root);
        if (EFI_ERROR(Status)) {
            continue;
        }
        
        Print(L"[SCAN] Checking filesystem %d...\n", i);
        ScanESP(Handles[i], Root);
        
        Root->Close(Root);
    }
    
    FreePool(Handles);
    
    Print(L"[DONE] Found %d operating system(s)\n\n", gOSCount);
    
    return EFI_SUCCESS;
}

// ============================================================================
// DEBUG: Print detected OS
// ============================================================================

VOID PrintDetectedOS(VOID) {
    Print(L"\n=== Detected Operating Systems ===\n");
    for (UINT32 i = 0; i < gOSCount; i++) {
        Print(L"%d. %s\n", i + 1, gDetectedOS[i].Name);
        Print(L"   Path: %s\n", gDetectedOS[i].BootPath);
    }
    Print(L"==================================\n\n");
}