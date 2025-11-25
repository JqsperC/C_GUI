#ifndef UI_H
#define UI_H

#include "../base/base.h"
#include "../renderer/renderer.h"

typedef struct {
    b32 left_mouse_down;

    i32 mouse_x;
    i32 mouse_y;

    u64 id;
    u64 active;
    u64 hot;
} ui_state;

typedef struct {
    b32 clicked;
    b32 hover;
    b32 pressed;
    b32 released;
} ui_signal;

typedef enum {
    UI_SIZEKIND_PIXELS = 0,
    UI_SIZEKIND_TEXT_CONTENT,
    UI_SIZEKIND_SUM_CHILDREN,
    UI_SIZEKIND_PARENT_PERCENT,

    UI_SIZEKIND_COUNT
} ui_sizekind;

typedef struct {
    ui_sizekind kind;
    f32 value;
} ui_size;

typedef enum {
    AXIS_X = 0,
    AXIS_Y,

    AXIS_COUNT
} ui_layout_axis;


typedef u32 ui_widget_flags;
enum {
    UI_WIDGETFLAG_CLICKABLE       = (1<<0),
    UI_WIDGETFLAG_HOVERABLE       = (1<<1),
    UI_WIDGETFLAG_DRAW_TEXT       = (1<<2),
    UI_WIDGETFLAG_DRAW_BORDER     = (1<<3),
    UI_WIDGETFLAG_DRAW_BACKGROUND = (1<<4),
    UI_WIDGETFLAG_VIEW_SCROLL     = (1<<5),
};

typedef struct UI_widget {
    struct UI_widget *first;
    struct UI_widget *last;
    struct UI_widget *next;
    struct UI_widget *prev;
    struct UI_widget *parent;

    struct UI_widget *hash_prev;
    struct UI_widget *hash_next;

    u64 id;
    u64 last_frame_touched;

    String8 string;

    ui_widget_flags flags;
    ui_size size[AXIS_COUNT];

    ui_layout_axis layout_direction;

    f32 computed_relative_position[AXIS_COUNT];
    f32 computed_size[AXIS_COUNT];

    Rectangle rect;

    f32 hot_t;
    f32 active_t;
} UI_widget;

typedef struct {
    i32 font_size;

    ColorRGBX bg;
    ColorRGBX fg;
    ColorRGBX hover;
    ColorRGBX active;
    ColorRGBX border;
    ColorRGBX debug;

    u32 text_padding[AXIS_COUNT];

} ui_style;

void UI_Initialize();

void UI_SetStyle(ui_style style);
void UI_StyleSetFont(char *file, i32 size);
void UI_StyleSetFGColor(ColorRGBX color);
void UI_StyleSetBGColor(ColorRGBX color);
void UI_StyleSetHoverColor(ColorRGBX color);
void UI_StyleSetActiveColor(ColorRGBX color);


UI_widget *UI_PushParent(UI_widget *widget, ui_layout_axis axis);
UI_widget *UI_PopParent();

u64 UI_Key();
u64 UI_KeyString(String8 str);

UI_widget *UI_MakeWidget(String8 label, ui_widget_flags flags, ui_size size[AXIS_COUNT]);
ui_signal UI_SignalFromWidget(UI_widget *widget);

void UI_Begin(i32 mouse_x, i32 mouse_y, b32 mouse_down, i32 width, i32 height);
void UI_End();

void UI_StartLayoutBlock(String8 label, ui_size size[2], ui_layout_axis layout_direction);
void UI_EndLayoutBlock();

void UI_StartGroup(String8 label, String8 button_label, ui_layout_axis layout_direction, b32 *show_group);
void UI_EndGroup();

void UI_Layout();
void UI_Render();

ui_signal UI_Button(String8 label, ui_size size[AXIS_COUNT]);
ui_signal UI_Slider(String8 label, float *value);

#endif
