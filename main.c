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

b32 EventLoop(sdl_ui_state *ui_state_old){
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
    Arena *arena_permanent = ArenaAllocate();

    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!SDL_CreateWindowAndRenderer("UI Test", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        printf("Failed to create Window/Rendere: %s\n", SDL_GetError());
        return -1;
    }
    
    if (!InitializeTextRenderer()){
        return -1;
    }
    if (!LoadFont("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf", 16)){
        return -1;
    }
    UI_Initialize();
    
    f32 seconds_per_frame = 1.0f / FRAMERATE;
    u64 last_counter = SDL_GetPerformanceCounter();
    UI_SetStyle(
            (ui_style)
            {.border = (ColorRGBX){.red = 255}}
            );
    UI_StyleSetFGColor((ColorRGBX){.red = 255, .green = 255, .blue = 255} );
    UI_StyleSetBGColor((ColorRGBX) {});
    UI_StyleSetHoverColor((ColorRGBX) {.blue = 255});
    UI_StyleSetActiveColor((ColorRGBX) {.red = 255});
    sdl_ui_state ui_state;
    dimension = SDLGetWindowSize(window);
    SDLResizeBuffer(arena_permanent, renderer, dimension.width, dimension.height);

    while(EventLoop(&ui_state)){

        FlushBuffer();



        UI_Begin(ui_state.mouse_x, ui_state.mouse_y, ui_state.left_mouse_down, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1);


        UI_widget *widget = UI_MakeWidget(0, NULL, (ui_size[AXIS_COUNT]){{.value = 1, .kind = UI_SIZEKIND_PARENT_PERCENT}, {.value = 200, .kind = UI_SIZEKIND_PIXELS}});
        UI_PushParent(widget, AXIS_X);
        
        widget = UI_MakeWidget(0, NULL, (ui_size[AXIS_COUNT]){{.value = 0.25, .kind = UI_SIZEKIND_PARENT_PERCENT}, {.value = 1, .kind = UI_SIZEKIND_PARENT_PERCENT}});
        widget = UI_MakeWidget(0, NULL, (ui_size[AXIS_COUNT]){{.value = 0.5, .kind = UI_SIZEKIND_PARENT_PERCENT}, {.value = 1, .kind = UI_SIZEKIND_PARENT_PERCENT}});
        widget = UI_MakeWidget(0, NULL, (ui_size[AXIS_COUNT]){{.value = 0.25, .kind = UI_SIZEKIND_PARENT_PERCENT}, {.value = 1, .kind = UI_SIZEKIND_PARENT_PERCENT}});

        UI_PopParent();
        widget = UI_MakeWidget(0, NULL, (ui_size[AXIS_COUNT]){{.value = 1, .kind = UI_SIZEKIND_PARENT_PERCENT}, {.value = 300, .kind = UI_SIZEKIND_PIXELS}});
        UI_PushParent(widget, AXIS_Y);
        widget = UI_MakeWidget(0, NULL, (ui_size[AXIS_COUNT]){{.value = 1, .kind = UI_SIZEKIND_PARENT_PERCENT}, {.value = 50, .kind = UI_SIZEKIND_PIXELS}});
        widget = UI_MakeWidget(0, NULL, (ui_size[AXIS_COUNT]){{.value = 1, .kind = UI_SIZEKIND_PARENT_PERCENT}, {.value = 50, .kind = UI_SIZEKIND_PIXELS}});
        UI_PopParent();
        UI_Layout();
        UI_Render();

        UI_End();
        SDLRenderBufferToWindow(renderer);

        //Framerate

        DEBUG_LOG("Rendered frame in %.6f ms\n", SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter()) * 1000);

        while (SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter()) < seconds_per_frame)
        {
            SDL_Delay((seconds_per_frame - SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter())) * 1000);
        }
        DEBUG_LOG("Running at %.2f fps\n", 1.0f / SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter()));
        last_counter = SDL_GetPerformanceCounter();
    }
    DestroyBackbuffer();
    ArenaRelease(arena_permanent);
}
