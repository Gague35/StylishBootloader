#ifndef UI_H
#define UI_H

#include <Uefi.h>

// Menu
VOID MenuMoveLeft(VOID);
VOID MenuMoveRight(VOID);
UINT32 MenuGetSelected(VOID);
VOID RenderCarousel(UINT32 ScreenWidth, UINT32 ScreenHeight);
VOID MenuUpdate(VOID);

// Input
typedef enum {
    INPUT_NONE,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_SELECT,
    INPUT_ESCAPE
} INPUT_ACTION;

INPUT_ACTION PollInput(VOID);

#endif