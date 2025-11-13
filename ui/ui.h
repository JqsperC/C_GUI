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
    i32 font_size;

    ColorRGBX bg;
    ColorRGBX fg;
    ColorRGBX hover;
    ColorRGBX active;

} ui_style;

void UI_StyleSetFont(TTF_Font *font);
void UI_StyleSetFGColor(ColorRGBX color);
void UI_StyleSetBGColor(ColorRGBX color);
void UI_StyleSetHoverColor(ColorRGBX color);
void UI_StyleSetActiveColor(ColorRGBX color);

void UI_Begin(i32 mouse_x, i32 mouse_y, b32 mouse_down);
void UI_End();

b32 UI_Button(char *text, i32 id, i32 pos_x, i32 pos_y, i32 width, i32 height);
void UI_Checkbox(char *text, i32 id, i32 pos_x, i32 pos_y, i32 size, b32 *value);
void UI_Selector(char **text, i32 id, i32 pos_x, i32 pos_y, i32 size, i32 n, i32 *selection);
void UI_Slider(char *text, i32 id, i32 pos_x, i32 pos_y, i32 width, i32 height, f32 min, f32 max, f32 *value);

#endif
