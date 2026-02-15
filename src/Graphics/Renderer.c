#include <Uefi.h>
#include <Library/UefiLib.h>
#include "Graphics.h"

// ============================================================================
// EFFET GLOW (LUEUR)
// ============================================================================

VOID DrawGlow(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, UINT32 Color, UINT32 Intensity) {
    // Extraire RGB de la couleur
    UINT32 R = (Color >> 16) & 0xFF;
    UINT32 G = (Color >> 8)  & 0xFF;
    UINT32 B = (Color >> 0)  & 0xFF;
    
    // Dessiner 3 couches de glow
    for (UINT32 Layer = 0; Layer < 3; Layer++) {
        UINT32 Expansion = (Layer + 1) * 8;  // Chaque couche est plus grande
        UINT32 Opacity = (Intensity * (3 - Layer)) / 3;  // Opacité décroissante
        
        // Calculer la couleur avec opacité
        UINT32 GlowR = (R * Opacity) / 255;
        UINT32 GlowG = (G * Opacity) / 255;
        UINT32 GlowB = (B * Opacity) / 255;
        
        UINT32 GlowColor = 0xFF000000 | (GlowR << 16) | (GlowG << 8) | GlowB;
        
        // Dessiner le rectangle étendu
        if (X >= Expansion && Y >= Expansion) {
            DrawFilledRectToBuffer(
                X - Expansion,
                Y - Expansion,
                Width + (Expansion * 2),
                Height + (Expansion * 2),
                GlowColor
            );
        }
    }
}

// ============================================================================
// RECTANGLE SCALÉ
// ============================================================================

VOID DrawRectScaled(UINT32 X, UINT32 Y, UINT32 BaseWidth, UINT32 BaseHeight, UINT32 Scale, UINT32 Color) {
    // Calculer nouvelle taille
    UINT32 ScaledWidth = (BaseWidth * Scale) / 100;
    UINT32 ScaledHeight = (BaseHeight * Scale) / 100;
    
    // Centrer le rectangle (pas coin haut-gauche)
    UINT32 FinalX = X - (ScaledWidth / 2);
    UINT32 FinalY = Y - (ScaledHeight / 2);
    
    DrawFilledRectToBuffer(FinalX, FinalY, ScaledWidth, ScaledHeight, Color);
}