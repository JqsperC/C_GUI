#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
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
v2f32 GetWindowSize();

ColorRGBX BlendRGBXARGB(ColorRGBX background, ColorARGB foreground);

void RenderRectangle(i32 pos_x, i32 pos_y, i32 width, i32 height,
                     ColorRGBX color);

void RenderRectOutlines(Rectangle rect, ColorRGBX colo );
void RenderRect(Rectangle rect, ColorRGBX color);
void SDLResizeBuffer(Arena *arena, SDL_Renderer *renderer, i32 width, i32 height);
void SDLRenderBufferToWindow(SDL_Renderer *renderer);

void SDLRenderARGBText(i32 offset_x, i32 offset_y, i32 height, SDL_Surface *text);



void FlushBuffer();

void RenderARGBBitmap(i32 pos_x, i32 pos_y, u32 *memory, i32 width, i32 height);
void RenderRGBXBitmap(i32 pos_x, i32 pos_y, u32 *memory, i32 width, i32 height);

// TEXT RENDERER

#define ASCII_NUM_GLYPHS 128

typedef struct {
    u8 *bitmap;
    i32 bitmap_width;
    i32 bitmap_height;
    i32 bearing_x;
    i32 bearing_y;
    i32 advance;
} glyph;

typedef enum {
    WRAP_KIND_NONE = 0,
    WRAP_KIND_CHAR,
    WRAP_KIND_WORD,

    WRAP_KIND_COUNT
} text_wrap_kind;

typedef enum {
    ALIGN_LEFT = 0,
    ALIGN_CENTER,
    ALIGN_RIGHT,

    ALIGN_KIND_COUNT
} text_align_kind;
b32 InitializeTextRenderer();
b32 LoadFont(char *filename, i32 size);
i32 RenderCharacter(i32 pos_x, i32 pos_y, u8 character, ColorRGBX color);
void RenderText(i32 pos_x, i32 pos_y, i32 width, i32 height, u8 *string, ColorRGBX color, text_wrap_kind wrap, text_align_kind align);

#endif
