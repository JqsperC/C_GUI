#include <stdint.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600 

#define FRAMERATE 60

#include "base/base.h"
#include "renderer/renderer.h"
#include "ui/ui.h"

typedef struct {
    b32 left_mouse_down;
    b32 left_mouse_was_down;
    b32 right_mouse_down;
    b32 right_mouse_was_down;

    i32 mouse_x;
    i32 mouse_y;

    i32 mouse_rel_x;
    i32 mouse_rel_y;
} sdl_ui_state;

b32 EventLoop(sdl_ui_state *ui_state_old, Arena *arena_framebuffer){
    SDL_Event event;
    sdl_ui_state ui_state_new = *ui_state_old;
    ui_state_new.left_mouse_was_down = ui_state_old->left_mouse_down;
    ui_state_new.right_mouse_was_down = ui_state_old->right_mouse_down;
    ui_state_new.mouse_rel_x = 0;
    ui_state_new.mouse_rel_y = 0;
    while (SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    return false;
                }
                break;
            case SDL_EVENT_MOUSE_MOTION:
                ui_state_new.mouse_x = event.motion.x;
                ui_state_new.mouse_y = event.motion.y;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == 1){
                    ui_state_new.left_mouse_down = event.button.down;
                }
                if (event.button.button == 2){
                    ui_state_new.left_mouse_down = event.button.down;
                }
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                {
                SDL_Window *window = SDL_GetWindowFromEvent(&event);
                sdl_window_dimension dimension = SDLGetWindowSize(window);
                SDL_Renderer *renderer = SDL_GetRenderer(window);
                SDLResizeBuffer(arena_framebuffer, renderer, dimension.width, dimension.height);
                }
        }
    }
    ui_state_new.mouse_rel_x = ui_state_new.mouse_x - ui_state_old->mouse_x;
    ui_state_new.mouse_rel_y = ui_state_new.mouse_y - ui_state_old->mouse_y;
    *ui_state_old = ui_state_new;
    return true;
}


static f32 SDLGetSecondsElapsed(u64 OldCounter, u64 CurrentCounter)
{
    return ((f32)(CurrentCounter - OldCounter) / (f32)(SDL_GetPerformanceFrequency()));
}

int main(){
    printf("%lu\n", sizeof(ColorARGB));
    SDL_Window *window;
    SDL_Renderer *renderer;
    sdl_window_dimension dimension;
    Arena *arena_framebuffer = ArenaAllocateFixedSize(1920 * 1080 * 4 * 10);

    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!SDL_CreateWindowAndRenderer("UI Test", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        printf("Failed to create Window/Rendere: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetWindowResizable(window, true);
    
    if (!InitializeTextRenderer()){
        return -1;
    }
    if (!LoadFont("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf", 16)){
        return -1;
    }
    UI_Initialize();
    
    f32 seconds_per_frame = 1.0f / FRAMERATE;
    u64 last_counter = SDL_GetPerformanceCounter();

    UI_SetStyle((ui_style)
            {
            .border = (ColorRGBX) {.green = 255, .blue = 255},
            .debug = (ColorRGBX) {.red = 255},
            .fg = (ColorRGBX) {.red = 255, .green = 255, .blue = 255},
            .bg = (ColorRGBX) {.red = 20, .green = 30, .blue = 60},
            .hover = (ColorRGBX) {.blue = 255},
            .active = (ColorRGBX) {.green = 255},
            });

    sdl_ui_state ui_state;
    dimension = SDLGetWindowSize(window);
    SDLResizeBuffer(arena_framebuffer, renderer, dimension.width, dimension.height);

    b32 show_sidebar_group1 = false;
    b32 show_sidebar_group2 = false;

    while(EventLoop(&ui_state, arena_framebuffer)){

        FlushBuffer();

        v2f32 window_size = GetWindowSize();
        UI_Begin(ui_state.mouse_x, ui_state.mouse_y, ui_state.left_mouse_down, window_size.x - 1, window_size.y - 1);
        UI_StartLayoutBlock("Block 1", (ui_size[2])
                {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}, {.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}}, AXIS_X);
        {
            UI_StartLayoutBlock("Block 2", (ui_size[2])
                    {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 0.3}, {.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}}, AXIS_Y);
            {
                UI_StartLayoutBlock("Block 3", (ui_size[2])
                        {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}, {.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 0.05}}, AXIS_X);
                {
                    UI_Button("Button 1", (ui_size[AXIS_COUNT])
                            {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 0.25},{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}});
                    UI_Button("Button 2", (ui_size[AXIS_COUNT])
                            {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 0.25},{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}});
                    UI_Button("Button 3", (ui_size[AXIS_COUNT])
                            {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 0.25},{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}});
                    UI_Button("BUtton 4", (ui_size[AXIS_COUNT])
                            {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 0.25},{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}});
                }
                UI_EndLayoutBlock();
                UI_StartGroup("Group 1", "Group 1Button", AXIS_Y, &show_sidebar_group1);
                {
                    if (show_sidebar_group1)
                    {
                        UI_MakeWidget("Group 1 Box 1", 0, (ui_size[AXIS_COUNT])
                                {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}, {.kind = UI_SIZEKIND_PIXELS, .value = 80}});
                        UI_MakeWidget("Group 1 Box 2", 0, (ui_size[AXIS_COUNT])
                                {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}, {.kind = UI_SIZEKIND_PIXELS, .value = 80}});
                        UI_MakeWidget("Group 1 Box 3", 0, (ui_size[AXIS_COUNT])
                                {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}, {.kind = UI_SIZEKIND_PIXELS, .value = 80}});
                    }
                }
                UI_EndGroup();
                UI_StartGroup("Group 2", "Group 2 Button", AXIS_Y, &show_sidebar_group2);
                {
                    if (show_sidebar_group2)
                    {
                        UI_MakeWidget("Group 2 Box 1", 0,(ui_size[AXIS_COUNT])
                                {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}, {.kind = UI_SIZEKIND_PIXELS, .value = 80}});
                        UI_MakeWidget("Group 2 Box 2", 0,(ui_size[AXIS_COUNT])
                                {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}, {.kind = UI_SIZEKIND_PIXELS, .value = 80}});
                        UI_MakeWidget("Group 2 Box 3", 0,(ui_size[AXIS_COUNT])
                                {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}, {.kind = UI_SIZEKIND_PIXELS, .value = 80}});
                        UI_MakeWidget("Group 2 Box 4", 0,(ui_size[AXIS_COUNT])
                                {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}, {.kind = UI_SIZEKIND_PIXELS, .value = 80}});
                    }
                }
                UI_EndGroup();
            }
            UI_EndLayoutBlock();
            UI_StartLayoutBlock("Block 4", (ui_size[2])
                    {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 0.7}, {.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1}}, AXIS_Y);
            {
                UI_StartLayoutBlock("Block 5", (ui_size[2])
                        {{.kind = UI_SIZEKIND_PARENT_PERCENT, .value = 1},{.kind = UI_SIZEKIND_PIXELS, .value = 100}}, AXIS_X);
                UI_EndLayoutBlock();
            }
            UI_EndLayoutBlock();

        }
        UI_EndLayoutBlock();
        UI_Layout();
        UI_Render();

        UI_End();
        SDLRenderBufferToWindow(renderer);

        //Framerate
        // DEBUG_LOG("Rendered frame in %.6f ms\n", SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter()) * 1000);
        while (SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter()) < seconds_per_frame)
        {
            SDL_Delay((seconds_per_frame - SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter())) * 1000);
        }
        // DEBUG_LOG("Running at %.2f fps\n", 1.0f / SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter()));
        last_counter = SDL_GetPerformanceCounter();
    }
    DestroyBackbuffer();
    ArenaRelease(arena_framebuffer);
}
