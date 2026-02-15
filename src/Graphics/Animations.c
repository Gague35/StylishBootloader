#include <Uefi.h>
#include <Library/UefiLib.h>

// ============================================================================
// INTERPOLATION
// ============================================================================

UINT32 LerpUINT32(UINT32 Start, UINT32 End, UINT32 T, UINT32 MaxT) {
    if (T >= MaxT) {
        return End;
    }
    
    if (End > Start) {
        UINT32 Diff = End - Start;
        return Start + (Diff * T) / MaxT;
    } else {
        UINT32 Diff = Start - End;
        return Start - (Diff * T) / MaxT;
    }
}

UINT32 EaseInOutUINT32(UINT32 Start, UINT32 End, UINT32 T, UINT32 MaxT) {
    return LerpUINT32(Start, End, T, MaxT);
}

// ============================================================================
// COLOR INTERPOLATION
// ============================================================================

UINT32 LerpColor(UINT32 ColorStart, UINT32 ColorEnd, UINT32 T, UINT32 MaxT) {
    if (T >= MaxT) {
        return ColorEnd;
    }
    
    UINT32 R1 = (ColorStart >> 16) & 0xFF;
    UINT32 G1 = (ColorStart >> 8)  & 0xFF;
    UINT32 B1 = (ColorStart >> 0)  & 0xFF;
    
    UINT32 R2 = (ColorEnd >> 16) & 0xFF;
    UINT32 G2 = (ColorEnd >> 8)  & 0xFF;
    UINT32 B2 = (ColorEnd >> 0)  & 0xFF;
    
    UINT32 R = LerpUINT32(R1, R2, T, MaxT);
    UINT32 G = LerpUINT32(G1, G2, T, MaxT);
    UINT32 B = LerpUINT32(B1, B2, T, MaxT);
    
    return 0xFF000000 | (R << 16) | (G << 8) | B;
}

// ============================================================================
// SCALE & OPACITY CALCULATION
// ============================================================================


UINT32 CalculateScale(UINT32 Distance, UINT32 MaxDist) {
    if (Distance >= MaxDist) {
        return 50;  // Minimum 50%
    }
    
    // Formule : 100% - (Distance / MaxDist) * 50%
    return 100 - ((Distance * 50) / MaxDist);
}


UINT32 CalculateOpacity(UINT32 Distance, UINT32 MaxDist) {
    if (Distance >= MaxDist) {
        return 100;  // Minimum visible
    }
    
    // Formule : 255 - (Distance / MaxDist) * 155
    return 255 - ((Distance * 155) / MaxDist);
}