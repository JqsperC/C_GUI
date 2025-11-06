#ifndef UI_H
#define UI_H

#include "../base/base.h"
#include "../renderer/renderer.h"

typedef struct {
    b32 left_mouse_down;

    i32 mouse_x;
    i32 mouse_y;

    i32 id;
    i32 active;
    i32 hot;
} ui_state;


typedef struct {
    TTF_Font *font;
    i32 font_siez;

    ColorRGBX bg;
    ColorRGBX fg;

} ui_config;

void UI_ConfigSetFont(TTF_Font *font);
void UI_ConfigSetFGColor(ColorRGBX color);
void UI_ConfigSetBGColor(ColorRGBX color);

void UI_Begin(i32 mouse_x, i32 mouse_y, b32 mouse_down);
void UI_End();

b32 UI_Button(char *text, i32 id, i32 pos_x, i32 pos_y, i32 width, i32 height);
void UI_Checkbox(char *text, i32 id, i32 pos_x, i32 pos_y, i32 size, b32 *value);

#endif
