#include <Uefi.h>
#include <Library/UefiLib.h>
#include "Graphics.h"

// ============================================================================
// EFFET GLOW (LUEUR)
// ============================================================================

VOID DrawGlow(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, UINT32 Color, UINT32 Intensity) {
    UINT32 R = (Color >> 16) & 0xFF;
    UINT32 G = (Color >> 8)  & 0xFF;
    UINT32 B = (Color >> 0)  & 0xFF;
    
    // Dessiner 3 couches de glow (du plus grand au plus petit pour l'ordre)
    for (UINT32 Layer = 3; Layer > 0; Layer--) {
        UINT32 Expansion = Layer * 12;  // Plus d'expansion (12 au lieu de 8)
        UINT32 Opacity = (Intensity * Layer) / 3;  // Opacité croissante
        
        UINT32 GlowR = (R * Opacity) / 255;
        UINT32 GlowG = (G * Opacity) / 255;
        UINT32 GlowB = (B * Opacity) / 255;
        
        UINT32 GlowColor = 0xFF000000 | (GlowR << 16) | (GlowG << 8) | GlowB;
        
        // Calculer la taille et position du glow (X,Y = CENTRE)
        UINT32 GlowWidth = Width + (Expansion * 2);
        UINT32 GlowHeight = Height + (Expansion * 2);
        
        // Convertir centre en coin haut-gauche
        INT32 GlowX = X - (GlowWidth / 2);
        INT32 GlowY = Y - (GlowHeight / 2);
        
        // Vérifier les bornes
        if (GlowX >= 0 && GlowY >= 0) {
            DrawFilledRectToBuffer(GlowX, GlowY, GlowWidth, GlowHeight, GlowColor);
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