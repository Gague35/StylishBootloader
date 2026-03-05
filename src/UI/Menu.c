#include <Uefi.h>
#include <Library/UefiLib.h>
#include "../Graphics/Graphics.h"
#include "../Boot/OSDetector.h"

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

VOID MenuButtonLeft(VOID);
VOID MenuButtonRight(VOID);


// ============================================================================
// CAROUSEL STATE
// ============================================================================

#define ANIM_DURATION 20

typedef struct {
    UINT32  SelectedIndex;
    INT32   AnimationOffset;
    UINT32  AnimationProgress;
    BOOLEAN IsAnimating;
    INT32 AnimationDirection;
} CAROUSEL_STATE;

CAROUSEL_STATE gCarousel = {0, 0, 0, FALSE, 0};
UINT32 gCurrentSpacing = 280;

// Bottom button states
typedef enum {
    BTN_SETTINGS = 0,
    BTN_BIOS = 1,
    BTN_SHUTDOWN = 2,
    BTN_COUNT = 3
} BOTTOM_BUTTON;

typedef struct {
    BOOLEAN InCarousel;         // TRUE = carousel, FALSE = buttons
    UINT32  SelectedButton;     // Which button (0-2)
    UINT32  ButtonAnimTimer;    // Animation timer (0-12 frames = 0.2s)
} MENU_STATE;

MENU_STATE gMenuState = {TRUE, 0, 0};  // Start in carousel

// Dynamic item count
UINT32 GetMaxItems(VOID) {
    return gOSCount > 0 ? gOSCount : 1;  // At least 1 item
}

// ============================================================================
// SHARED PULSE TIMER (for carousel and buttons)
// ============================================================================

UINT32 gPulseTimer = 0;  // Global, shared between carousel and buttons

// ============================================================================
// NAVIGATION
// ============================================================================

VOID MenuMoveLeft(VOID) {
    if (!gMenuState.InCarousel) {
        MenuButtonLeft();
        return;
    }
    
    UINT32 MaxItems = GetMaxItems();

    // If an animation is in progress, we adjust from the current position
    if (gCarousel.IsAnimating) {
        
        // Change target
        if (gCarousel.SelectedIndex == 0) {
            gCarousel.SelectedIndex = MaxItems - 1;
        } else {
            gCarousel.SelectedIndex--;
        }
        
        extern UINT32 gCurrentSpacing;
        gCarousel.AnimationOffset = gCarousel.AnimationOffset + gCurrentSpacing;

        // If the offset exceeds the limit, it is limited.
        if (gCarousel.AnimationOffset > gCurrentSpacing) {
            gCarousel.AnimationOffset = gCurrentSpacing;
        }
        
        gCarousel.AnimationDirection = -1;
        gCarousel.AnimationProgress = 0;
        // IsAnimating stay TRUE
        
    } else {
        if (gCarousel.SelectedIndex == 0) {
            gCarousel.SelectedIndex = MaxItems - 1;
        } else {
            gCarousel.SelectedIndex--;
        }
        
        gCarousel.AnimationOffset = gCurrentSpacing;
        gCarousel.AnimationDirection = -1;
        gCarousel.AnimationProgress = 0;
        gCarousel.IsAnimating = TRUE;
    }
}



VOID MenuMoveRight(VOID) {
    if (!gMenuState.InCarousel) {
        MenuButtonRight();
        return;
    }
    
    UINT32 MaxItems = GetMaxItems();
    
    if (gCarousel.IsAnimating) {
        // Change target
        gCarousel.SelectedIndex = (gCarousel.SelectedIndex + 1) % MaxItems;
        
        // Adjust offset from current position
        gCarousel.AnimationOffset = gCarousel.AnimationOffset - gCurrentSpacing;
        
        if (gCarousel.AnimationOffset < -gCurrentSpacing) {
            gCarousel.AnimationOffset = -gCurrentSpacing;
        }
        
        gCarousel.AnimationDirection = 1;
        gCarousel.AnimationProgress = 0;
        // IsAnimating stays TRUE
        
    } else {
        // No animation in progress, start new one
        gCarousel.SelectedIndex = (gCarousel.SelectedIndex + 1) % MaxItems;
        
        gCarousel.AnimationOffset = -gCurrentSpacing;
        gCarousel.AnimationDirection = 1;
        gCarousel.AnimationProgress = 0;
        gCarousel.IsAnimating = TRUE;
    }
}

UINT32 MenuGetSelected(VOID) {
    return gCarousel.SelectedIndex;
}

// Set initial selected index
VOID MenuSetSelected(UINT32 Index) {
    UINT32 MaxItems = GetMaxItems();
    if (Index < MaxItems) {
        gCarousel.SelectedIndex = Index;
    }
}

// ============================================================================
// BUTTON NAVIGATION
// ============================================================================

VOID MenuMoveUp(VOID) {
    if (!gMenuState.InCarousel) {
        // From buttons to carousel
        gMenuState.InCarousel = TRUE;
    }
}

VOID MenuMoveDown(VOID) {
    if (gMenuState.InCarousel) {
        // From carousel to buttons
        gMenuState.InCarousel = FALSE;
        gMenuState.SelectedButton = BTN_SETTINGS;  // Default to Settings
    }
}

VOID MenuButtonLeft(VOID) {
    if (gMenuState.SelectedButton == 0) {
        gMenuState.SelectedButton = BTN_COUNT - 1;
    } else {
        gMenuState.SelectedButton--;
    }
    gMenuState.ButtonAnimTimer = 0;
}

VOID MenuButtonRight(VOID) {
    gMenuState.SelectedButton = (gMenuState.SelectedButton + 1) % BTN_COUNT;
    gMenuState.ButtonAnimTimer = 0;
}

// ============================================================================
// UPDATE
// ============================================================================

VOID MenuUpdate(VOID) {
    // Carousel animation
    if (gCarousel.IsAnimating) {
        gCarousel.AnimationProgress++;
        
        if (gCarousel.AnimationDirection > 0) {
            gCarousel.AnimationOffset = EaseInOutUINT32(
                gCurrentSpacing,
                0,
                gCarousel.AnimationProgress,
                ANIM_DURATION
            );
        } else {
            gCarousel.AnimationOffset = -EaseInOutUINT32(
                gCurrentSpacing,
                0,
                gCarousel.AnimationProgress,
                ANIM_DURATION
            );
        }
    }
    
    // ======== BUTTON GLOW ANIMATION ========
    // Always increment when on buttons (for continuous pulse)
    if (!gMenuState.InCarousel) {
        gMenuState.ButtonAnimTimer++;  // ← Toujours incrémenter, pas de limite
    }
}

// ============================================================================
// RENDER BOTTOM BUTTONS
// ============================================================================

VOID RenderBottomButtons(UINT32 ScreenWidth, UINT32 ScreenHeight) {
    // Scale button sizes
    UINT32 ButtonSize = 80;
    UINT32 ButtonSpacing = 120;
    UINT32 CenterX = ScreenWidth / 2;
    UINT32 ButtonY = ScreenHeight - 450;
    
    // Get pulse timer from carousel (shared)
    // IMPORTANT: Use the SAME timer as carousel for synchronization
    extern UINT32 gPulseTimer;  // We'll make this global
    
    for (UINT32 i = 0; i < BTN_COUNT; i++) {
        INT32 ButtonX = CenterX + ((INT32)i - 1) * ButtonSpacing;
        BOOLEAN IsSelected = (!gMenuState.InCarousel && gMenuState.SelectedButton == i);

        UINT32 Color = IsSelected ? RGB(200, 50, 50) : RGB(60, 60, 60);
        
        // ======== GLOW (selected only) ========
        if (IsSelected) {
        // Animation scale: 80% → 100% over 6 frames (rapide + immédiatement visible)
            UINT32 AnimScale = 100;  // Default to 100%
            if (gMenuState.ButtonAnimTimer < 6) {
                // Part de 80% et va jusqu'à 100%
                AnimScale = 80 + (gMenuState.ButtonAnimTimer * 20) / 6;
            }
            
            // Pulse amount (synchronized with carousel)
            UINT32 PulsePhase = gPulseTimer;
            if (PulsePhase > 60) PulsePhase = 120 - PulsePhase;
            UINT32 PulsePercent = (PulsePhase * 5) / 60;  // 0-5%
            
            // Combined scale: animation + pulse
            UINT32 FinalScale = AnimScale + PulsePercent;
            if (FinalScale > 105) FinalScale = 105;  // Cap at 105%
            
            // Calculate glow size with scale
            UINT32 GlowSize = (ButtonSize * FinalScale) / 100;
            
            // Draw 2-layer glow (lighter than carousel)
            DrawGlowLayers(ButtonX, ButtonY, GlowSize, GlowSize, 
                          RGB(255, 80, 80), 180, 2);
        }
        
        // ======== BUTTON (no pulse, stays fixed) ========
        DrawRectScaled(ButtonX, ButtonY, ButtonSize, ButtonSize, 100, Color);
        
        // ======== ICON ========
        CHAR16* Icon = L"?";
        CHAR16* Label = L"";
        
        switch (i) {
            case BTN_SETTINGS:
                Icon = L"@";  // Gear
                Label = L"Settings";
            break;
            case BTN_BIOS:
                Icon = L"$";  // Heart/Chip
                Label = L"BIOS";
            break;
            case BTN_SHUTDOWN:
                Icon = L"#";  // Power
                Label = L"Shutdown";
            break;
}
      
        UINT32 IconColor = IsSelected ? RGB(255, 255, 255) : RGB(120, 120, 120);
        
        DrawStringCenteredScaled(Icon, ButtonX, ButtonY - 16, IconColor, 4);
        
        // ======== LABEL (if selected) ========
        if (IsSelected) {
            DrawStringCenteredScaled(Label, ButtonX, ButtonY + ButtonSize/2 + 25, 
                                    RGB(255, 255, 255), 2);
        }
    }
}

// ============================================================================
// RENDERING
// ============================================================================

VOID RenderCarousel(UINT32 ScreenWidth, UINT32 ScreenHeight) {
    UINT32 CentreX = ScreenWidth / 2;
    UINT32 CentreY = ScreenHeight / 2;

    UINT32 BaseWidth  = 300;
    UINT32 BaseHeight = 180;
    UINT32 Spacing    = 420;
    gCurrentSpacing = Spacing;

    gPulseTimer = (gPulseTimer + 1) % 120;
    
    INT32 PulsePhase = gPulseTimer;
    if (PulsePhase > 60) {
        PulsePhase = 120 - PulsePhase;
    }
    UINT32 PulseAmount = (PulsePhase * 5) / 60;

    UINT32 MaxItems = GetMaxItems();

    // BOUCLE : Dessiner tous les items
    for (UINT32 i = 0; i < MaxItems; i++) { 
        INT32 RelativeIndex = (INT32)i - (INT32)gCarousel.SelectedIndex;
        INT32 BaseX = CentreX + (RelativeIndex * Spacing);
        INT32 FinalX = BaseX + gCarousel.AnimationOffset;

        if (FinalX < -300 || FinalX > (INT32)ScreenWidth + 300) continue;

        UINT32 Distance = (FinalX > (INT32)CentreX) ? 
                          (FinalX - CentreX) : 
                          (CentreX - FinalX);

        UINT32 Scale = CalculateScale(Distance, 500);
        UINT32 Opacity = CalculateOpacity(Distance, 500);

        if (Distance < 50) {
            Scale = Scale + PulseAmount;
        }

        // Glow
        if (Distance < 50) {
            UINT32 GlowIntensity = 255 - ((Distance * 155) / 50);
            UINT32 GlowWidth = (BaseWidth * Scale) / 100;
            UINT32 GlowHeight = (BaseHeight * Scale) / 100;
            DrawGlow(FinalX, CentreY, GlowWidth, GlowHeight, 
                     RGB(255, 80, 80), GlowIntensity);
        }

        // Rectangle
        UINT32 BaseColor = (Distance < 50) ? RGB(200, 50, 50) : RGB(80, 80, 80);
        UINT32 R = (BaseColor >> 16) & 0xFF;
        UINT32 G = (BaseColor >> 8)  & 0xFF;
        UINT32 B = (BaseColor >> 0)  & 0xFF;
        R = (R * Opacity) / 255;
        G = (G * Opacity) / 255;
        B = (B * Opacity) / 255;
        UINT32 FinalColor = 0xFF000000 | (R << 16) | (G << 8) | B;

        DrawRectScaled(FinalX, CentreY, BaseWidth, BaseHeight, Scale, FinalColor);
        
        // ----------------------------------------------------------------
        // NOM SOUS LA CASE (seulement si sélectionné)
        // ----------------------------------------------------------------
        if (Distance < 50) {
            // Récupérer le nom
            CHAR16* Label = L"";
            
            if (i < gOSCount) {
                Label = gDetectedOS[i].Name;
            } else if (i == gOSCount) {
                Label = L"Settings";
            } else {
                Label = L"Shutdown";
            }
            
            // Texte blanc, taille 2x (16px)
            UINT32 TextScale = 2;
            UINT32 TextColor = RGB(255, 255, 255);
            
            // Position SOUS le rectangle
            INT32 TextY = CentreY + (BaseHeight / 2) + 40;
            
            DrawStringCenteredScaled(Label, FinalX, TextY, TextColor, TextScale);
        }
    }

    // Draw bottom buttons
    RenderBottomButtons(ScreenWidth, ScreenHeight);
}