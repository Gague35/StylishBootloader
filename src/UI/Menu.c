#include <Uefi.h>
#include <Library/UefiLib.h>
#include "../Graphics/Graphics.h"

// ============================================================================
// MENU STATE
// ============================================================================

#define MAX_MENU_ITEMS 4

typedef struct {
    UINT32 SelectedIndex;
    UINT32 PreviousIndex;
    UINT32 AnimationProgress;
    UINT32 AnimationDuration;
    UINT32 FadeProgress;
} MENU_STATE;

MENU_STATE gMenuState = {0, 0, 0, 30, 0};

// ============================================================================
// MENU LOGIC
// ============================================================================

VOID MenuMoveUp(VOID) {
    if (gMenuState.SelectedIndex > 0) {
        gMenuState.PreviousIndex = gMenuState.SelectedIndex;
        gMenuState.SelectedIndex--;
        gMenuState.AnimationProgress = 0;
    }
}

VOID MenuMoveDown(VOID) {
    if (gMenuState.SelectedIndex < MAX_MENU_ITEMS - 1) {
        gMenuState.PreviousIndex = gMenuState.SelectedIndex;
        gMenuState.SelectedIndex++;
        gMenuState.AnimationProgress = 0;
    }
}

UINT32 MenuGetSelected(VOID) {
    return gMenuState.SelectedIndex;
}

VOID MenuUpdate(VOID) {
    if (gMenuState.AnimationProgress < gMenuState.AnimationDuration) {
        gMenuState.AnimationProgress++;
    }
    
    if (gMenuState.FadeProgress < 60) {
        gMenuState.FadeProgress++;
    }
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
    
    UINT32 SelectedColor = RGB(200, 50, 50);
    UINT32 InactiveColor = RGB(60, 60, 60);
    UINT32 TransitionColor = RGB(120, 50, 50);
    
    UINT32 FadeFactor = gMenuState.FadeProgress;
    
    for (UINT32 i = 0; i < MAX_MENU_ITEMS; i++) {
        UINT32 Y = StartY + i * (ItemHeight + ItemSpacing);
        
        UINT32 Color;
        
        if (i == gMenuState.SelectedIndex) {
            if (gMenuState.AnimationProgress < gMenuState.AnimationDuration) {
                Color = LerpColor(
                    TransitionColor,
                    SelectedColor,
                    gMenuState.AnimationProgress,
                    gMenuState.AnimationDuration
                );
            } else {
                Color = SelectedColor;
            }
        } else if (i == gMenuState.PreviousIndex && gMenuState.AnimationProgress < gMenuState.AnimationDuration) {
            Color = LerpColor(
                SelectedColor,
                InactiveColor,
                gMenuState.AnimationProgress,
                gMenuState.AnimationDuration
            );
        } else {
            Color = InactiveColor;
        }
        
        // Apply fade-in
        if (FadeFactor < 60) {
            UINT32 R = (Color >> 16) & 0xFF;
            UINT32 G = (Color >> 8)  & 0xFF;
            UINT32 B = (Color >> 0)  & 0xFF;
            
            R = (R * FadeFactor) / 60;
            G = (G * FadeFactor) / 60;
            B = (B * FadeFactor) / 60;
            
            Color = 0xFF000000 | (R << 16) | (G << 8) | B;
        }
        
        DrawFilledRectToBuffer(StartX, Y, ItemWidth, ItemHeight, Color);
    }
}