#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <sys/mman.h>

#include "../base/base.h"


typedef struct {
    i32 width;
    i32 height;
} sdl_window_dimension;

typedef struct {
    SDL_Texture *texture;
    void *memory;
    i32 width;
    i32 height;
    i32 pitch;
    i32 bytes_per_pixel;
} sdl_offscreen_buffer;

typedef struct {
    union {
        struct {
            u8 red;
            u8 green;
            u8 blue;
            u8 alpha;
        };
        u8 bytes[4];
        u32 pixel;
    };
} ColorARGB;

typedef struct {
    union {
        struct {
            u8 X;
            u8 blue;
            u8 green;
            u8 red;
        };
        u8 bytes[4];
        u32 pixel;
    };
} ColorRGBX;

void DestroyBackbuffer();
sdl_window_dimension SDLGetWindowSize(SDL_Window* window);

ColorRGBX RemoveAlpha(ColorRGBX background, ColorARGB foreground);

void RenderRectangle(i32 pos_x, i32 pos_y, i32 width, i32 height,
                     ColorRGBX color);

void RenderRect(Rectangle rect, ColorRGBX color);
void SDLResizeBuffer(Arena *arena, SDL_Renderer *renderer, i32 width, i32 height);
void SDLRenderBufferToWindow(SDL_Renderer *renderer);

void SDLRenderARGBText(i32 offset_x, i32 offset_y, i32 height, SDL_Surface *text);

void SDLRenderText(const u8 *text,
        TTF_Font *font,
        SDL_Color color,
        i32 pos_x,
        i32 pos_y,
        i32 width,
        i32 height);

void FlushBuffer();

#endif
