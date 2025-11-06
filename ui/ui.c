#ifndef UI_C
#define UI_C

#include "ui.h"

static ui_state uistate;
static ui_config uiconfig;

void UI_ConfigSetFont(TTF_Font *font){
    uiconfig.font = font;
}
void UI_Begin(i32 mouse_x, i32 mouse_y, b32 mouse_down){
    uistate.left_mouse_down = mouse_down;
    uistate.mouse_x = mouse_x;
    uistate.mouse_y = mouse_y;

    uistate.id = 0;
    uistate.hot = 0;
}

void UI_End(){
    if (!uistate.left_mouse_down) {
        uistate.active = 0;
    } else if (!uistate.active){
        uistate.active = -1;
    }
}

b32 UI_Button(char *text, i32 id, i32 pos_x, i32 pos_y, i32 width, i32 height) {
    if (uistate.mouse_x > pos_x &&
        uistate.mouse_y > pos_y &&
        uistate.mouse_x < pos_x + width &&
        uistate.mouse_y < pos_y + height) {
        uistate.hot = id;
        if (uistate.active == 0 && uistate.left_mouse_down) {
            uistate.active = id;
        }
    }
    ColorRGBX color = {.red = 255};
    if (uistate.active == id && uistate.hot == id) {
        color = (ColorRGBX) {.green = 255};
    } else if (uistate.hot == id) {
        color = (ColorRGBX) {.blue = 255};
    }
    RenderRectangle(pos_x, pos_y, width, height, color);
    return id == uistate.active;
}
#endif
