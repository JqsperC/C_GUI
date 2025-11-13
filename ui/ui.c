#ifndef UI_C
#define UI_C

#include "ui.h"
#include <stdio.h>
static ui_state uistate;
static ui_style uistyle;

void UI_StyleSetFont(TTF_Font *font){
    uistyle.font = font;
}

void UI_StyleSetFGColor(ColorRGBX color){
    uistyle.fg = color;
}

void UI_StyleSetBGColor(ColorRGBX color){
    uistyle.bg = color;
}

void UI_StyleSetHoverColor(ColorRGBX color){
    uistyle.hover = color;
}

void UI_StyleSetActiveColor(ColorRGBX color){
    uistyle.active = color;
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
    ColorRGBX color = uistyle.fg;
    if (uistate.active == id && uistate.hot == id) {
        color = uistyle.active;
    } else if (uistate.hot == id) {
        color = uistyle.hover;
    } else if (uistate.active == id) {
        color = uistyle.active;
    }
    RenderRectangle(pos_x, pos_y, width, height, color);
    b32 button_clicked = uistate.active == id && uistate.hot == id && !uistate.left_mouse_down;
    return button_clicked;
}


void UI_Checkbox(char *text, i32 id, i32 pos_x, i32 pos_y, i32 size, b32 *value){
    if (uistate.mouse_x > pos_x &&
        uistate.mouse_y > pos_y &&
        uistate.mouse_x < pos_x + size &&
        uistate.mouse_y < pos_y + size) {
        uistate.hot = id;
        if (uistate.active == 0 && uistate.left_mouse_down) {
            uistate.active = id;
        }
    }
    b32 clicked = uistate.active == id && uistate.hot == id && !uistate.left_mouse_down;
    if (clicked) {
        *value = !*value;
    }
    i32 gap = 5;
    RenderRectangle(pos_x, pos_y, size, size, uistyle.fg);
    RenderRectangle(pos_x + gap, pos_y + gap, size - 2 * gap, size - 2*gap, uistyle.bg);
    if (*value){
        RenderRectangle(pos_x + 2 * gap, pos_y + 2 * gap, size - 4 * gap, size - 4 * gap, uistyle.fg);
    }
}

void UI_Selector(char **text, i32 id, i32 pos_x, i32 pos_y, i32 size, i32 n, i32 *selection){
    i32 gap = 5;
    for (i32 i = 0; i < n; i++){
       if (uistate.mouse_x > pos_x &&
        uistate.mouse_y > (pos_y + (size + gap) * i)       &&
        uistate.mouse_x < (pos_x + size) &&
        uistate.mouse_y < (pos_y + (size + gap) * (i + 1))) {
        uistate.hot = id;
        if (uistate.active == 0 && uistate.left_mouse_down) {
            uistate.active = id;
        }
        b32 clicked = uistate.active == id && uistate.hot == id && !uistate.left_mouse_down;
        if (clicked) {
            *selection = i;
        }
    }
    }
    for (i32 i = 0; i < n; i++){
        RenderRectangle(pos_x, 
                (pos_y + i * size + i * gap),
                size,
                size,
                uistyle.fg
                );
        RenderRectangle(pos_x + gap, 
                (pos_y + i * size + (i + 1) * gap),
                size - 2 * gap,
                size - 2 * gap,
                uistyle.bg
                );
        if (*selection == i){
            RenderRectangle(pos_x + 2 * gap,
                    (pos_y + i * size + (i + 2) * gap),
                    size - 4 * gap,
                    size - 4 * gap,
                    uistyle.fg
                    );

        }
    }
}
void UI_Slider(char *text, i32 id, i32 pos_x, i32 pos_y, i32 width, i32 height, f32 min, f32 max, f32 *value){
    i32 gap = 5;
    if (uistate.mouse_x > pos_x &&
            uistate.mouse_y > pos_y &&
            uistate.mouse_x < pos_x + width &&
            uistate.mouse_y < pos_y + height) {
        uistate.hot = id;
        if (uistate.active == 0 && uistate.left_mouse_down) {
            uistate.active = id;
        }
    }
    if (uistate.active == id && uistate.left_mouse_down) {
        f32 pos_rel = (f32)(uistate.mouse_x - pos_x - 0.5 * height - 2 * gap) /
                      (f32)(width - height - 4 * gap);
        if (pos_rel < 0) pos_rel = 0;
        if (pos_rel > 1) pos_rel = 1;
        *value = min + (max - min) * pos_rel;
        printf("%f %f\n", *value, pos_rel);
    }
    f32 pos_rel = (*value - min) / (max - min);
    f32 dot_pos_x = pos_rel * (width - height);
    RenderRectangle(pos_x, pos_y, width, height, uistyle.fg);
    RenderRectangle(pos_x + gap, pos_y + gap, width - 2 * gap, height - 2 * gap, uistyle.bg);
    RenderRectangle(pos_x + dot_pos_x + 2 * gap, pos_y + 2 * gap, height - 4 * gap, height - 4 * gap, uistyle.fg);
}


#endif
