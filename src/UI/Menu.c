#include <Uefi.h>
#include <Library/UefiLib.h>
#include "../Graphics/Graphics.h"

// ============================================================================
// CAROUSEL STATE
// ============================================================================

#define MAX_ITEMS 4
#define SPACING 280      // Distance entre chaque case
#define ANIM_DURATION 20 

typedef struct {
    UINT32  SelectedIndex;      // Index sélectionné (0-3)
    INT32   AnimationOffset;    // Offset d'animation (-SPACING à +SPACING)
    UINT32  AnimationProgress;  // Compteur (0 à ANIM_DURATION)
    BOOLEAN IsAnimating;        // En cours d'animation ?
    INT32 AnimationDirection;
} CAROUSEL_STATE;

CAROUSEL_STATE gCarousel = {0, 0, 0, FALSE, 0};

// ============================================================================
// NAVIGATION
// ============================================================================

VOID MenuMoveLeft(VOID) {
    // If an animation is in progress, we adjust from the current position
    if (gCarousel.IsAnimating) {
        
        // Change target
        if (gCarousel.SelectedIndex == 0) {
            gCarousel.SelectedIndex = MAX_ITEMS - 1;
        } else {
            gCarousel.SelectedIndex--;
        }
        
        gCarousel.AnimationOffset = gCarousel.AnimationOffset + SPACING;
        
        // If the offset exceeds the limit, it is limited.
        if (gCarousel.AnimationOffset > SPACING) {
            gCarousel.AnimationOffset = SPACING;
        }
        
        gCarousel.AnimationDirection = -1;
        gCarousel.AnimationProgress = 0;
        // IsAnimating stay TRUE
        
    } else {
        if (gCarousel.SelectedIndex == 0) {
            gCarousel.SelectedIndex = MAX_ITEMS - 1;
        } else {
            gCarousel.SelectedIndex--;
        }
        
        gCarousel.AnimationOffset = SPACING;
        gCarousel.AnimationDirection = -1;
        gCarousel.AnimationProgress = 0;
        gCarousel.IsAnimating = TRUE;
    }
}

VOID MenuMoveRight(VOID) {

    if (gCarousel.IsAnimating) {

        gCarousel.SelectedIndex = (gCarousel.SelectedIndex + 1) % MAX_ITEMS;
        gCarousel.AnimationOffset = gCarousel.AnimationOffset - SPACING;
        
        if (gCarousel.AnimationOffset < -SPACING) {
            gCarousel.AnimationOffset = -SPACING;
        }
        
        gCarousel.AnimationDirection = 1;
        gCarousel.AnimationProgress = 0;
        
    } else {
        gCarousel.SelectedIndex = (gCarousel.SelectedIndex + 1) % MAX_ITEMS;
        
        gCarousel.AnimationOffset = -SPACING;
        gCarousel.AnimationDirection = 1;
        gCarousel.AnimationProgress = 0;
        gCarousel.IsAnimating = TRUE;
    }
}

UINT32 MenuGetSelected(VOID) {
    return gCarousel.SelectedIndex;
}

// ============================================================================
// UPDATE
// ============================================================================

VOID MenuUpdate(VOID) {
    if (!gCarousel.IsAnimating) return;
    
    gCarousel.AnimationProgress++;
    
    // Interpolate according to the stored direction
    if (gCarousel.AnimationDirection > 0) {
        // Animation to the right (from +SPACING to 0)
        gCarousel.AnimationOffset = EaseInOutUINT32(
            SPACING,
            0,
            gCarousel.AnimationProgress,
            ANIM_DURATION
        );
    } else {
        // Animation to the left (from -SPACING to 0)
        gCarousel.AnimationOffset = -EaseInOutUINT32(
            SPACING,
            0,
            gCarousel.AnimationProgress,
            ANIM_DURATION
        );
    }
    
    if (gCarousel.AnimationProgress >= ANIM_DURATION) {
        gCarousel.AnimationOffset = 0;
        gCarousel.IsAnimating = FALSE;
    }
}

// ============================================================================
// RENDERING
// ============================================================================

VOID RenderCarousel(UINT32 ScreenWidth, UINT32 ScreenHeight) {
    UINT32 CentreX = ScreenWidth / 2;
    UINT32 CentreY = ScreenHeight / 2;

    UINT32 BaseWidth  = 200;
    UINT32 BaseHeight = 120;

    // Timer for pulse animation
    static UINT32 PulseTimer = 0;
    PulseTimer = (PulseTimer + 1) % 120;
    
    INT32 PulsePhase = PulseTimer;
    if (PulsePhase > 60) {
        PulsePhase = 120 - PulsePhase;
    }
    UINT32 PulseAmount = (PulsePhase * 5) / 60;

    // ------------------------------------------------------------------------
    // LOOP : Glow THEN Rectangle
    // ------------------------------------------------------------------------
    for (UINT32 i = 0; i < MAX_ITEMS; i++) {
        INT32 RelativeIndex = (INT32)i - (INT32)gCarousel.SelectedIndex;
        INT32 BaseX = CentreX + (RelativeIndex * SPACING);
        INT32 FinalX = BaseX + gCarousel.AnimationOffset;

        if (FinalX < -300 || FinalX > (INT32)ScreenWidth + 300) continue;

        UINT32 Distance = (FinalX > (INT32)CentreX) ? 
                          (FinalX - CentreX) : 
                          (CentreX - FinalX);

        UINT32 Scale = CalculateScale(Distance, 500);
        UINT32 Opacity = CalculateOpacity(Distance, 500);

        // Appliquer le pulse si très proche du centre
        if (Distance < 50) {
            Scale = Scale + PulseAmount;
        }

        // ----------------------------------------------------------------
        // 1. DRAW GLOW (only if VERY close to the center)
        // ----------------------------------------------------------------
        if (Distance < 50) { 
            UINT32 GlowIntensity = 255 - ((Distance * 155) / 50);
            
            UINT32 GlowWidth = (BaseWidth * Scale) / 100;
            UINT32 GlowHeight = (BaseHeight * Scale) / 100;
            
            DrawGlow(FinalX, CentreY, GlowWidth, GlowHeight, 
                     RGB(255, 80, 80), GlowIntensity);
        }

        // ----------------------------------------------------------------
        // DRAW RECTANGLE
        // ----------------------------------------------------------------
        UINT32 BaseColor = (Distance < 50) ? RGB(200, 50, 50) : RGB(80, 80, 80);

        UINT32 R = (BaseColor >> 16) & 0xFF;
        UINT32 G = (BaseColor >> 8)  & 0xFF;
        UINT32 B = (BaseColor >> 0)  & 0xFF;

        R = (R * Opacity) / 255;
        G = (G * Opacity) / 255;
        B = (B * Opacity) / 255;

        UINT32 FinalColor = 0xFF000000 | (R << 16) | (G << 8) | B;

        DrawRectScaled(FinalX, CentreY, BaseWidth, BaseHeight, Scale, FinalColor);
        
        DrawRectScaled(FinalX, CentreY, BaseWidth, BaseHeight, Scale, FinalColor);
        
        // ----------------------------------------------------------------
        // DRAW TEXT (with scale and opacity)
        // ----------------------------------------------------------------
        CHAR16* Label = L"";
        switch (i) {
            case 0: Label = L"Windows"; break;
            case 1: Label = L"Linux";   break;
            case 2: Label = L"BIOS"; break;
            case 3: Label = L"Shutdown"; break;
        }
        
        // Calculate the scale of the text (same as the rectangle)
        UINT32 TextScale = Scale / 50;  // Scale 100 → 2x, Scale 50 → 1x
        if (TextScale < 1) TextScale = 1;  // Minimum 1x
        
        // Color with opacity (like the rectangle)
        UINT32 BaseTextColor = (Distance < 50) ? RGB(255, 255, 255) : RGB(150, 150, 150);
        
        UINT32 TR = (BaseTextColor >> 16) & 0xFF;
        UINT32 TG = (BaseTextColor >> 8)  & 0xFF;
        UINT32 TB = (BaseTextColor >> 0)  & 0xFF;
        
        TR = (TR * Opacity) / 255;
        TG = (TG * Opacity) / 255;
        TB = (TB * Opacity) / 255;
        
        UINT32 TextColor = 0xFF000000 | (TR << 16) | (TG << 8) | TB;
        
        // Text position (centered on the rectangle)
        INT32 TextY = CentreY - (4 * TextScale); // Vertically centered
        
        DrawStringCenteredScaled(Label, FinalX, TextY, TextColor, TextScale);
    }
}