#include <Uefi.h>
#include <Library/UefiLib.h>
#include "../Graphics/Graphics.h"

// ============================================================================
// CAROUSEL STATE
// ============================================================================

#define MAX_ITEMS 4
#define SPACING 280      // Distance entre chaque case
#define ANIM_DURATION 30 // 20 frames = 0.3 sec à 60 FPS

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
    // Si une animation est en cours, on ajuste depuis la position actuelle
    if (gCarousel.IsAnimating) {
        // Calculer la "distance restante" dans l'animation actuelle
        // L'offset actuel indique où on est vraiment
        // On va partir de là pour la nouvelle animation
        
        // Changer de cible
        if (gCarousel.SelectedIndex == 0) {
            gCarousel.SelectedIndex = MAX_ITEMS - 1;
        } else {
            gCarousel.SelectedIndex--;
        }
        
        // L'offset actuel devient le point de départ
        // On veut aller vers la droite, donc offset positif
        // Mais on part de l'offset actuel (qui peut être négatif)
        gCarousel.AnimationOffset = gCarousel.AnimationOffset + SPACING;
        
        // Si l'offset dépasse, on le limite
        if (gCarousel.AnimationOffset > SPACING) {
            gCarousel.AnimationOffset = SPACING;
        }
        
        gCarousel.AnimationDirection = -1;
        gCarousel.AnimationProgress = 0;
        // IsAnimating reste TRUE
        
    } else {
        // Pas d'animation en cours, comportement normal
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
    // Si une animation est en cours, on ajuste depuis la position actuelle
    if (gCarousel.IsAnimating) {
        // Changer de cible
        gCarousel.SelectedIndex = (gCarousel.SelectedIndex + 1) % MAX_ITEMS;
        
        // L'offset actuel devient le point de départ
        // On veut aller vers la gauche, donc offset négatif
        gCarousel.AnimationOffset = gCarousel.AnimationOffset - SPACING;
        
        // Si l'offset dépasse, on le limite
        if (gCarousel.AnimationOffset < -SPACING) {
            gCarousel.AnimationOffset = -SPACING;
        }
        
        gCarousel.AnimationDirection = 1;
        gCarousel.AnimationProgress = 0;
        // IsAnimating reste TRUE
        
    } else {
        // Pas d'animation en cours, comportement normal
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
    
    // Interpoler selon la direction mémorisée
    if (gCarousel.AnimationDirection > 0) {
        // Animation vers la droite (de +SPACING vers 0)
        gCarousel.AnimationOffset = EaseInOutUINT32(
            SPACING,
            0,
            gCarousel.AnimationProgress,
            ANIM_DURATION
        );
    } else {
        // Animation vers la gauche (de -SPACING vers 0)
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

    // Timer pour l'animation pulse du glow
    static UINT32 PulseTimer = 0;
    PulseTimer = (PulseTimer + 1) % 120;
    
    INT32 PulsePhase = PulseTimer;
    if (PulsePhase > 60) {
        PulsePhase = 120 - PulsePhase;
    }
    UINT32 PulseAmount = (PulsePhase * 5) / 60;

    // ------------------------------------------------------------------------
    // BOUCLE UNIQUE : Rectangle PUIS Glow
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

        // ----------------------------------------------------------------
        // 1. DESSINER LE RECTANGLE D'ABORD (sans animation)
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

        // ----------------------------------------------------------------
        // 2. DESSINER LE GLOW PAR-DESSUS (avec animation pulse)
        // ----------------------------------------------------------------
        if (Distance < 100) {
            UINT32 GlowIntensity = 255 - ((Distance * 155) / 100);
            
            // Le glow pulse
            UINT32 GlowScale = Scale;
            if (Distance < 50) {
                GlowScale = Scale + PulseAmount;
            }
            
            UINT32 GlowWidth = (BaseWidth * GlowScale) / 100;
            UINT32 GlowHeight = (BaseHeight * GlowScale) / 100;
            
            DrawGlow(FinalX, CentreY, GlowWidth, GlowHeight, 
                     RGB(255, 70, 60), GlowIntensity);
        }
    }
}