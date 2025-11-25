#ifndef WIDGETS_C
#define WIDGETS_C

#include "ui.h"

ui_signal UI_Button(String8 label, ui_size size[AXIS_COUNT]) {
    UI_widget *widget = UI_MakeWidget(label,
            UI_WIDGETFLAG_CLICKABLE |
            UI_WIDGETFLAG_HOVERABLE |
            UI_WIDGETFLAG_DRAW_BACKGROUND |
            UI_WIDGETFLAG_DRAW_BORDER|
            UI_WIDGETFLAG_DRAW_TEXT,
            size);
    ui_signal signal = UI_SignalFromWidget(widget);
    return signal;
}

void UI_StartGroup(String8 label, String8 button_label, ui_layout_axis layout_direction, b32 *show_group)
{
    ui_size layout_axis = {.kind = UI_SIZEKIND_SUM_CHILDREN, .value = 0};
    ui_size other_axis  = {.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1};
    ui_size size_group[AXIS_COUNT];
    size_group[layout_direction] = layout_axis;
    size_group[AXIS_Y - layout_direction] = other_axis;

    UI_StartLayoutBlock(label, size_group, layout_direction);
    if (UI_Button(button_label, (ui_size[AXIS_COUNT])
                {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1},
                {.kind = UI_SIZEKIND_PIXELS, .value = 50}}).clicked)
    {
        *show_group = !*show_group;
    }
}

void UI_EndGroup()
{
    UI_EndLayoutBlock();
}


#endif
