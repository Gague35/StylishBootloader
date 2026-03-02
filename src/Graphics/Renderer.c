#include <Uefi.h>
#include <Library/UefiLib.h>
#include "Graphics.h"

// ============================================================================
// GLOW EFFECT
// ============================================================================

VOID DrawGlow(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, UINT32 Color, UINT32 Intensity) {
    UINT32 R = (Color >> 16) & 0xFF;
    UINT32 G = (Color >> 8)  & 0xFF;
    UINT32 B = (Color >> 0)  & 0xFF;
    
    // Draw 3 layers of glow (from largest to smallest)
    for (UINT32 Layer = 3; Layer > 0; Layer--) {
        UINT32 Expansion = Layer * 12;  // 36, 24, 12
        
        UINT32 Opacity = (Intensity * (4 - Layer)) / 3;
        
        // Apply opacity to the colors
        UINT32 GlowR = (R * Opacity) / 255;
        UINT32 GlowG = (G * Opacity) / 255;
        UINT32 GlowB = (B * Opacity) / 255;
        
        UINT32 GlowColor = 0xFF000000 | (GlowR << 16) | (GlowG << 8) | GlowB;
        
        // Calculate the size and position (X,Y = CENTER)
        UINT32 GlowWidth = Width + (Expansion * 2);
        UINT32 GlowHeight = Height + (Expansion * 2);
        
        // Convert center to top-left corner
        INT32 GlowX = X - (GlowWidth / 2);
        INT32 GlowY = Y - (GlowHeight / 2);
        
        if (GlowX >= 0 && GlowY >= 0) {
            DrawFilledRectToBuffer(GlowX, GlowY, GlowWidth, GlowHeight, GlowColor);
        }
    }
}

// ============================================================================
// SCALED RECTANGLE
// ============================================================================

VOID DrawRectScaled(UINT32 X, UINT32 Y, UINT32 BaseWidth, UINT32 BaseHeight, UINT32 Scale, UINT32 Color) {
    // Calculate new size
    UINT32 ScaledWidth = (BaseWidth * Scale) / 100;
    UINT32 ScaledHeight = (BaseHeight * Scale) / 100;
    
    // Center the rectangle (not the top left corner)
    UINT32 FinalX = X - (ScaledWidth / 2);
    UINT32 FinalY = Y - (ScaledHeight / 2);
    
    DrawFilledRectToBuffer(FinalX, FinalY, ScaledWidth, ScaledHeight, Color);
}