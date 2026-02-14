#include <Uefi.h>
#include <Library/UefiLib.h>
#include "../Graphics/Graphics.h"

// ============================================================================
// MENU STATE
// ============================================================================

#define MAX_MENU_ITEMS 4

typedef struct {
    UINT32 SelectedIndex;
    UINT32 ItemCount;
} MENU_STATE;

MENU_STATE gMenuState = {0, MAX_MENU_ITEMS};

// ============================================================================
// MENU LOGIC
// ============================================================================

VOID MenuMoveUp(VOID) {
    if (gMenuState.SelectedIndex > 0) {
        gMenuState.SelectedIndex--;
    }
}

VOID MenuMoveDown(VOID) {
    if (gMenuState.SelectedIndex < gMenuState.ItemCount - 1) {
        gMenuState.SelectedIndex++;
    }
}

UINT32 MenuGetSelected(VOID) {
    return gMenuState.SelectedIndex;
}

// ============================================================================
// MENU RENDERING
// ============================================================================

VOID RenderMenu(UINT32 ScreenWidth, UINT32 ScreenHeight) {
    UINT32 ItemWidth = 400;
    UINT32 ItemHeight = 80;
    UINT32 ItemSpacing = 20;
    UINT32 TotalHeight = (ItemHeight + ItemSpacing) * MAX_MENU_ITEMS - ItemSpacing;
    
    UINT32 StartX = (ScreenWidth - ItemWidth) / 2;
    UINT32 StartY = (ScreenHeight - TotalHeight) / 2;
    
    // Dessiner chaque option
    for (UINT32 i = 0; i < MAX_MENU_ITEMS; i++) {
        UINT32 Y = StartY + i * (ItemHeight + ItemSpacing);
        
        // Couleur : Rouge si sélectionné, gris sinon
        UINT32 Color;
        if (i == gMenuState.SelectedIndex) {
            Color = RGB(200, 50, 50);  // Rouge
        } else {
            Color = RGB(60, 60, 60);   // Gris foncé
        }
        
        DrawFilledRectToBuffer(StartX, Y, ItemWidth, ItemHeight, Color);
    }
}