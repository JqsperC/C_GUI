#include <stdint.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600 

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

int main(){
    printf("%lu\n", sizeof(ColorARGB));
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    sdl_window_dimension dimension;

    Arena *arena_permanent = ArenaAllocate();

    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }

    if (!TTF_Init()) {
        printf("Failed to initialize TTF: %s\n", SDL_GetError());
        return -1;
    }
    if (!SDL_CreateWindowAndRenderer("UI Test", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        printf("Failed to create Window/Rendere: %s\n", SDL_GetError());
        return -1;
    }

    font = TTF_OpenFont("/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Regular.ttf", 20);
    if (!font) {
        printf("Failed to open font: %s\n", SDL_GetError());
        return -1;
    }
    UI_StyleSetFont(font);
    UI_StyleSetFGColor((ColorRGBX) {.green = 255});
    UI_StyleSetBGColor((ColorRGBX) {});
    UI_StyleSetHoverColor((ColorRGBX) {.blue = 255});
    UI_StyleSetActiveColor((ColorRGBX) {.red = 255});
    sdl_ui_state ui_state;
    dimension = SDLGetWindowSize(window);
    SDLResizeBuffer(arena_permanent, renderer, dimension.width, dimension.height);

    f32 value = 0;
    i32 test = 0;
    b32 check = false;
    while(EventLoop(&ui_state)){
        FlushBuffer();

        UI_Begin(ui_state.mouse_x, ui_state.mouse_y, ui_state.left_mouse_down);

        UI_Slider(NULL, 100, 100, 100, 300, 70, 0, 100, &value);
        UI_Selector(NULL, 69, 100, 250, 70, 3, &test); 
        if (UI_Button(NULL, 22, 250, 250, 200, 100)) {
            printf("click\n");
        }
        UI_Checkbox(NULL, 23, 250, 400, 70, &check);

        UI_End();

        SDLRenderBufferToWindow(renderer);
    }
    DestroyBackbuffer();
    ArenaRelease(arena_permanent);
}
