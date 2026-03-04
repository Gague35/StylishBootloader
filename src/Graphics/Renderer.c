#include <Uefi.h>
#include <Library/UefiLib.h>
#include "Graphics.h"

// ============================================================================
// GLOW EFFECT
// ============================================================================

// Draw glow with custom layer count
VOID DrawGlowLayers(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, 
                    UINT32 Color, UINT32 Intensity, UINT32 LayerCount) {
    
    for (UINT32 Layer = LayerCount; Layer >= 1; Layer--) {
        // Expansion per layer
        UINT32 Expansion = Layer * 7;
        
        UINT32 GlowWidth  = Width  + (Expansion * 2);
        UINT32 GlowHeight = Height + (Expansion * 2);
        
        // Opacity calculation (adjusted for variable layers)
        UINT32 Opacity = (Intensity * (LayerCount + 1 - Layer)) / LayerCount;
        
        // Apply opacity to color
        UINT32 R = ((Color >> 16) & 0xFF) * Opacity / 255;
        UINT32 G = ((Color >> 8)  & 0xFF) * Opacity / 255;
        UINT32 B = ((Color >> 0)  & 0xFF) * Opacity / 255;
        
        UINT32 GlowColor = 0xFF000000 | (R << 16) | (G << 8) | B;
        
        // Position (centered)
        INT32 GlowX = X - (GlowWidth  / 2);
        INT32 GlowY = Y - (GlowHeight / 2);
        
        DrawFilledRectToBuffer(GlowX, GlowY, GlowWidth, GlowHeight, GlowColor);
    }
}

// Original 3-layer glow (for carousel)
VOID DrawGlow(UINT32 X, UINT32 Y, UINT32 Width, UINT32 Height, 
              UINT32 Color, UINT32 Intensity) {
    DrawGlowLayers(X, Y, Width, Height, Color, Intensity, 3);
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