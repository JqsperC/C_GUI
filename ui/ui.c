#ifndef UI_C
#define UI_C

#include "ui.h"

static UI_widget *widget_list[256];
static i32 widget_list_idx = 0;
static UI_widget *GetWidgetByID(u64 id)
{
    for (i32 i = 0; i < widget_list_idx; i++) 
    {
        if (widget_list[i]->id == id) {
            return widget_list[i];
        }
    }
    return NULL;
}
static b32 AddWidget(UI_widget *widget)
{
    if (widget_list_idx < 256)
    {
        widget_list[widget_list_idx] = widget;
        widget_list_idx++;
        return true;
    }
    return false;
}

static ui_state uistate;
static ui_style uistyle;

static UI_widget *root;
static UI_widget *parent;

static Arena *widget_arena;
static Arena *string_arena;

static i32 current_id;

void UI_Initialize()
{
    widget_arena = ArenaAllocate();
    string_arena = ArenaAllocate();
    if (!widget_arena)
    {
        DEBUG_LOG("Failed to initialize UI System\n");
    }
}

void UI_SetStyle(ui_style style){
    uistyle = style;
}

void UI_StyleSetFont(char *file, i32 size){
    LoadFont(file, size);
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

void UI_Begin(i32 mouse_x, i32 mouse_y, b32 mouse_down, i32 width, i32 height){
    ArenaClear(string_arena);
    current_id = 0;
    uistate.left_mouse_down = mouse_down;
    uistate.mouse_x = mouse_x;
    uistate.mouse_y = mouse_y;

    uistate.hot = 0;

    root = NULL;
    parent = NULL;

    ui_size size[AXIS_COUNT];
    size[AXIS_X].kind = UI_SIZEKIND_PIXELS;
    size[AXIS_X].value = width;
    size[AXIS_Y].kind = UI_SIZEKIND_PIXELS;
    size[AXIS_Y].value = height;
    UI_widget *widget = UI_MakeWidget(MakeString(string_arena, "Root widget"), 0, size);

    root = widget;
    parent = widget;
    widget->layout_direction = AXIS_Y;
}

void UI_End(){
    if (!uistate.left_mouse_down) {
        uistate.active = 0;
    } else if (!uistate.active){
        uistate.active = 0;
    }
}

UI_widget *UI_PushParent(UI_widget *widget, ui_layout_axis layout_direction){
    widget->layout_direction = layout_direction;
    widget->parent = parent;
    parent = widget;
    return parent;
}

UI_widget *UI_PopParent(){
    parent = parent->parent;
    return parent;
}

u64 UI_Key()
{
    return current_id++;
}

u64 UI_KeyString(String8 string)
{
    u64 key = hash(string);
    return key;
}

UI_widget *UI_MakeWidget(String8 label, ui_widget_flags flags, ui_size size[AXIS_COUNT]){
    u64 id = UI_KeyString(label);
    UI_widget *widget = GetWidgetByID(id);
    if (!widget)
    {
        widget = ArenaPushZero(widget_arena, sizeof(UI_widget));
        if (!widget) {
            return NULL;
        }
        AddWidget(widget);
    }
    widget->id = id;
    widget->flags = flags;
    widget->string = label;
    widget->size[AXIS_X] = size[AXIS_X]; 
    widget->size[AXIS_Y] = size[AXIS_Y]; 

    widget->parent = parent;
    widget->first = NULL;
    widget->last = NULL;
    widget->next = NULL;
    widget->prev = NULL;
    if (!parent)
    {
        return widget;
    }
    if (widget->parent->last)
    {
        widget->prev = widget->parent->last;
        widget->prev->next = widget;
        parent->last = widget;
    } else
    {
        parent->first = widget;
        parent->last = widget;
    }
    return widget;
}

void LayoutStandaloneSize(UI_widget *widget)
{
    if (!widget)
    {
        return;
    }
    switch (widget->size[AXIS_X].kind)
    {
        case UI_SIZEKIND_PIXELS:
        {
            widget->computed_size[AXIS_X] = widget->size[AXIS_X].value;
        }
        case UI_SIZEKIND_TEXT_CONTENT:
        {
            break;
        }
        default:
        {
            break;
        }
    }
    switch (widget->size[AXIS_Y].kind)
    {
        case UI_SIZEKIND_PIXELS:
         {
             widget->computed_size[AXIS_Y] = widget->size[AXIS_Y].value;
         }
        case UI_SIZEKIND_TEXT_CONTENT:
        {
            break;
        }
        default:
        {
            break;
        }
    }
    LayoutStandaloneSize(widget->next);
    LayoutStandaloneSize(widget->first);
}

void LayoutUpwardsDependentSize(UI_widget *widget)
{
    if (!widget)
    {
        return;
    }
    i32 parent_size[AXIS_COUNT];
    if (widget->parent) {
        parent_size[AXIS_X] = widget->parent->computed_size[AXIS_X];
        parent_size[AXIS_Y] = widget->parent->computed_size[AXIS_Y];
    }
    switch (widget->size[AXIS_X].kind)
    {
        case UI_SIZEKIND_PARENT_PERCENT:
            widget->computed_size[AXIS_X] = widget->size[AXIS_X].value * parent_size[AXIS_X];
            break;
        default:
            break;
    }
    switch (widget->size[AXIS_Y].kind)
    {
        case UI_SIZEKIND_PARENT_PERCENT:
            widget->computed_size[AXIS_Y] = widget->size[AXIS_Y].value * parent_size[AXIS_Y];
            break;
        default:
            break;
    }
    LayoutUpwardsDependentSize(widget->first);
    LayoutUpwardsDependentSize(widget->next);
}

void LayoutDownwardDependentSize(UI_widget *widget)
{
    if (!widget)
    {
        return;
    }
    LayoutDownwardDependentSize(widget->first);
    LayoutDownwardDependentSize(widget->next);

    switch (widget->size[AXIS_X].kind)
    {
        case UI_SIZEKIND_SUM_CHILDREN:
        {
            i32 size_x_children_sum = 0;
            for (UI_widget *child = widget->first; child != NULL; child = child->next)
            {
                size_x_children_sum += child->computed_size[AXIS_X];
            }
            widget->computed_size[AXIS_X] = size_x_children_sum;
            break;
        }
        default:
            break;
    }
    switch (widget->size[AXIS_Y].kind)
    {
        case UI_SIZEKIND_SUM_CHILDREN:
        {
            i32 size_y_children_sum = 0;
            for (UI_widget *child = widget->first; child != NULL; child = child->next)
            {
                size_y_children_sum += child->computed_size[AXIS_Y];
            }
            widget->computed_size[AXIS_Y] = size_y_children_sum;
            break;
        }
        default:
            break;
    }
}

void LayoutCalculatePosition(UI_widget *widget)
{
    if (!widget)
    {
        return;
    }

    if (!widget->parent)
    {
        widget->computed_relative_position[AXIS_X] = 0;
        widget->computed_relative_position[AXIS_Y] = 0;
        Rectangle rect = {.x = widget->computed_relative_position[AXIS_X],
                          .y = widget->computed_relative_position[AXIS_Y],
                          .width = widget->computed_size[AXIS_X],
                          .height = widget->computed_size[AXIS_Y]};

        widget->rect = rect;
    } else
    {
        switch (widget->parent->layout_direction)
        {
            case AXIS_X:
                {
                    widget->computed_relative_position[AXIS_X] = 0;
                    widget->computed_relative_position[AXIS_Y] = 0;
                    if (widget->prev)
                    {
                        widget->computed_relative_position[AXIS_X] = widget->prev->computed_size[AXIS_X] + widget->prev->computed_relative_position[AXIS_X];
                    }
                }
                break;
            case AXIS_Y:
                {
                    widget->computed_relative_position[AXIS_X] = 0;
                    widget->computed_relative_position[AXIS_Y] = 0;
                    if (widget->prev)
                    {
                        widget->computed_relative_position[AXIS_Y] = widget->prev->computed_size[AXIS_Y] + widget->prev->computed_relative_position[AXIS_Y];
                    }
                }
                break;
            default:
                break;
        }
        Rectangle rect = {.x = widget->computed_relative_position[AXIS_X] + widget->parent->rect.x,
                          .y = widget->computed_relative_position[AXIS_Y] + widget->parent->rect.y,
                          .width = widget->computed_size[AXIS_X],
                          .height = widget->computed_size[AXIS_Y]};
        widget->rect = rect;
    }
    LayoutCalculatePosition(widget->first);
    LayoutCalculatePosition(widget->next);
}

void UI_Layout() {
    LayoutStandaloneSize(root);
    LayoutUpwardsDependentSize(root);
    LayoutDownwardDependentSize(root);
    LayoutCalculatePosition(root);
}

void RenderLayout(UI_widget *widget)
{
    if (!widget)
    {
        return;
    }
    if (UI_WIDGETFLAG_DRAW_BACKGROUND & widget->flags)
    {
        RenderRect(widget->rect, uistyle.bg);
    }

    if (UI_WIDGETFLAG_DRAW_BORDER & widget->flags)
    {
        RenderRectOutlines(widget->rect, uistyle.border);
    }

    if (UI_WIDGETFLAG_HOVERABLE & widget->flags)
    {
        if (uistate.hot == widget->id && uistate.active == 0)
        {
            RenderRect(widget->rect, uistyle.hover);
        }
    }

    if (UI_WIDGETFLAG_CLICKABLE & widget->flags)
    {
        if (uistate.active == widget->id)
        {
            RenderRect(widget->rect, uistyle.active);
        }
    }
    if (UI_WIDGETFLAG_DRAW_TEXT & widget->flags)
    {
        RenderTextRect(widget->rect, widget->string, uistyle.fg, WRAP_KIND_WORD, HORIZONTAL_ALIGN_CENTER, VERTICAL_ALIGN_CENTER);
    }
    RenderLayout(widget->first);
    RenderLayout(widget->next);
}

void LayoutDebugRects(UI_widget *widget)
{
    if (!widget)
    {
        return;
    }
    RenderRectOutlines(widget->rect, uistyle.debug);
    LayoutDebugRects(widget->first);
    LayoutDebugRects(widget->next);
    
}

void UI_Render() 
{
    RenderLayout(root);
#ifdef DEBUG
    LayoutDebugRects(root);
#endif
}

ui_signal UI_SignalFromWidget(UI_widget *widget)
{
    ui_signal signal = {0};
    if (UI_WIDGETFLAG_CLICKABLE & widget->flags)
    {
        if (uistate.mouse_x > widget->rect.x &&
                uistate.mouse_y > widget->rect.y &&
                uistate.mouse_x < widget->rect.x + widget->rect.width &&
                uistate.mouse_y < widget->rect.y + widget->rect.height)
        {
            uistate.hot = widget->id;
            if (uistate.left_mouse_down &&
                uistate.active == 0)
            {
                uistate.active = widget->id;
                signal.pressed = true;
            }
        }
        if (uistate.active == widget->id && uistate.hot == widget->id && !uistate.left_mouse_down)
        {
            signal.clicked = true;
        }
    }
    if (UI_WIDGETFLAG_HOVERABLE & widget->flags)
    {
        if (uistate.hot == widget->id && uistate.active == 0)
        {
            signal.hover = true;
        }
    }
    return signal;
}

void UI_StartLayoutBlock(String8 label, ui_size size[2], ui_layout_axis layout_direction)
{
    UI_widget *widget = UI_MakeWidget(label, 0, size);
    UI_PushParent(widget, layout_direction);
}

void UI_EndLayoutBlock()
{
    UI_PopParent();
}

void UI_StartGroup(String8 label, String8 button_label, ui_layout_axis layout_direction, b32 *show_group)
{
    ui_size layout_axis = {.kind = UI_SIZEKIND_SUM_CHILDREN, .value = 0};
    ui_size other_axis  = {.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1};
    ui_size size_group[AXIS_COUNT];
    size_group[layout_direction] = layout_axis;
    size_group[AXIS_Y - layout_direction] = other_axis;

    UI_StartLayoutBlock(label, size_group, layout_direction);
    if (UI_Button(button_label, (ui_size[AXIS_COUNT]){{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1},{.kind = UI_SIZEKIND_PIXELS, .value = 50}}).clicked)
    {
        *show_group = !*show_group;
    }
}

void UI_EndGroup()
{
    UI_EndLayoutBlock();
}

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

#endif
