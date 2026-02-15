#include <Uefi.h>
#include <Library/UefiLib.h>
#include "../Graphics/Graphics.h"

// ============================================================================
// CAROUSEL STATE
// ============================================================================

#define MAX_ITEMS 4
#define SPACING 280      // Distance entre chaque case
#define ANIM_DURATION 30 // 30 frames = 0.5 sec à 60 FPS

typedef struct {
    UINT32  SelectedIndex;      // Index sélectionné (0-3)
    INT32   AnimationOffset;    // Offset d'animation (-SPACING à +SPACING)
    UINT32  AnimationProgress;  // Compteur (0 à ANIM_DURATION)
    BOOLEAN IsAnimating;        // En cours d'animation ?
} CAROUSEL_STATE;

CAROUSEL_STATE gCarousel = {0, 0, 0, FALSE};

// ============================================================================
// NAVIGATION
// ============================================================================

VOID MenuMoveLeft(VOID) {
    if (gCarousel.IsAnimating) return;  // Ignorer si déjà en animation
    
    // Passer à l'option précédente (circulaire)
    if (gCarousel.SelectedIndex == 0) {
        gCarousel.SelectedIndex = MAX_ITEMS - 1;  // 0 → 3
    } else {
        gCarousel.SelectedIndex--;
    }
    
    // Lancer animation vers la DROITE (cases glissent à droite)
    gCarousel.AnimationOffset = -SPACING;  // Commence décalé à gauche
    gCarousel.AnimationProgress = 0;
    gCarousel.IsAnimating = TRUE;
}

VOID MenuMoveRight(VOID) {
    if (gCarousel.IsAnimating) return;
    
    // Passer à l'option suivante (circulaire)
    gCarousel.SelectedIndex = (gCarousel.SelectedIndex + 1) % MAX_ITEMS;
    
    // Lancer animation vers la GAUCHE
    gCarousel.AnimationOffset = SPACING;  // Commence décalé à droite
    gCarousel.AnimationProgress = 0;
    gCarousel.IsAnimating = TRUE;
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
    
    // Interpoler l'offset vers 0
    gCarousel.AnimationOffset = LerpUINT32(
        (gCarousel.AnimationOffset < 0) ? -SPACING : SPACING,
        0,
        gCarousel.AnimationProgress,
        ANIM_DURATION
    );
    
    // Fin d'animation
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
    
    UINT32 BaseWidth = 200;
    UINT32 BaseHeight = 120;
    
    // PASSE 1 : Dessiner les glows (arrière-plan)
    for (UINT32 i = 0; i < MAX_ITEMS; i++) {
        INT32 RelativeIndex = (INT32)i - (INT32)gCarousel.SelectedIndex;
        INT32 BaseX = CentreX + (RelativeIndex * SPACING);
        INT32 FinalX = BaseX + gCarousel.AnimationOffset;
        
        if (FinalX < -300 || FinalX > (INT32)ScreenWidth + 300) {
            continue;
        }
        
        UINT32 Distance = (FinalX > (INT32)CentreX) ? 
                          (FinalX - CentreX) : 
                          (CentreX - FinalX);
        
        UINT32 Scale = CalculateScale(Distance, 500);
        
        // Dessiner glow seulement si au centre
        if (Distance < 50) {
            DrawGlow(FinalX, CentreY, 
                     (BaseWidth * Scale) / 100, 
                     (BaseHeight * Scale) / 100, 
                     RGB(200, 50, 50), 
                     150);  // Augmenté l'intensité
        }
    }
    
    // PASSE 2 : Dessiner les rectangles (premier plan)
    for (UINT32 i = 0; i < MAX_ITEMS; i++) {
        INT32 RelativeIndex = (INT32)i - (INT32)gCarousel.SelectedIndex;
        INT32 BaseX = CentreX + (RelativeIndex * SPACING);
        INT32 FinalX = BaseX + gCarousel.AnimationOffset;
        
        if (FinalX < -300 || FinalX > (INT32)ScreenWidth + 300) {
            continue;
        }
        
        UINT32 Distance = (FinalX > (INT32)CentreX) ? 
                          (FinalX - CentreX) : 
                          (CentreX - FinalX);
        
        UINT32 Scale = CalculateScale(Distance, 500);
        UINT32 Opacity = CalculateOpacity(Distance, 500);
        
        UINT32 BaseColor = (Distance < 50) ? 
                           RGB(200, 50, 50) : 
                           RGB(80, 80, 80);
        
        UINT32 R = (BaseColor >> 16) & 0xFF;
        UINT32 G = (BaseColor >> 8)  & 0xFF;
        UINT32 B = (BaseColor >> 0)  & 0xFF;
        
        R = (R * Opacity) / 255;
        G = (G * Opacity) / 255;
        B = (B * Opacity) / 255;
        
        UINT32 FinalColor = 0xFF000000 | (R << 16) | (G << 8) | B;
        
        DrawRectScaled(FinalX, CentreY, BaseWidth, BaseHeight, Scale, FinalColor);
    }
}