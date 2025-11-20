#ifndef RENDERER_C
#define RENDERER_C


#include "renderer.h"

static sdl_offscreen_buffer backbuffer;

const ColorRGBX white = {.red = 255, .green = 255, .blue = 255};
const ColorRGBX black = {.red = 0, .green = 0, .blue = 0};
const ColorRGBX red = {.red = 255, .green = 0, .blue = 0};
const ColorRGBX green = {.red = 0, .green = 255, .blue = 0};
const ColorRGBX blue = {.red = 0, .green = 0, .blue = 255};

sdl_window_dimension SDLGetWindowSize(SDL_Window* window) {
    sdl_window_dimension result;
    SDL_GetWindowSize(window, &result.width, &result.height);
    return result;
}

ColorRGBX BlendRGBXARGB(ColorRGBX background, ColorARGB foreground) {
    f32 fg = foreground.alpha / 255.0;
    f32 bg = 1 - fg;
    ColorRGBX res = {
        .red = background.red * bg + foreground.red * fg,
        .green = background.green * bg + foreground.green * fg,
        .blue = background.blue * bg + foreground.blue * fg,
    };
    return res;
}

void RenderRectOutlines(Rectangle rect, ColorRGBX color){
    u32 *pixels = backbuffer.memory;
    i32 x = rect.x;
    i32 y = rect.y;
    i32 w = rect.width;
    i32 h = rect.height;
    if (y < backbuffer.height)
    {
        for (i32 i = x; i < x + w && i < backbuffer.width; i++)
        {
            pixels[i + y * backbuffer.width] = color.pixel;
        }
    }
    if (y + h < backbuffer.height)
    {
        for (i32 i = x; i < x + w && i < backbuffer.width; i++)
        {
            pixels[i + (y + h) * backbuffer.width] = color.pixel;
        }
    }

    if (x < backbuffer.width)
    {
        for (i32 i = y; i < y + h && i < backbuffer.height; i++)
        {
            pixels[x + i * backbuffer.width] = color.pixel;
        }
    }
    if (x + w < backbuffer.width)
    {
        for (i32 i = y; i < y + h && i < backbuffer.height; i++)
        {
            pixels[x + w + i * backbuffer.width] = color.pixel;
        }
    }
}

void RenderRectangle(i32 pos_x, i32 pos_y, i32 width, i32 height, ColorRGBX color){
    ColorRGBX *pixel = (ColorRGBX*) backbuffer.memory;
    for (i32 x = 0; x < width && x + pos_x < backbuffer.width; x++){
        for (i32 y = 0; y < height && y + pos_y < backbuffer.height; y++){
            pixel[(y + pos_y) * backbuffer.width + x + pos_x] = color;
        }
    }
}

void DestroyBackbuffer(){
    if (backbuffer.texture){
        SDL_DestroyTexture(backbuffer.texture);
    }
}

void RenderRect(Rectangle rect, ColorRGBX color){
    ColorRGBX *pixel = (ColorRGBX*) backbuffer.memory;
    for (i32 x = 0; x < rect.width && x + rect.x < backbuffer.width; x++){
        for (i32 y = 0; y < rect.height && y + rect.y < backbuffer.height; y++){
            pixel[(y + rect.y) * backbuffer.width + x + rect.x] = color;
        }
    }
}

void SDLResizeBuffer(Arena *arena, SDL_Renderer *renderer, i32 width, i32 height){
    i32 bytes_per_pixel = 4;
    if (backbuffer.texture){
        SDL_DestroyTexture(backbuffer.texture);
    }
    backbuffer.texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBX8888, 
                                SDL_TEXTUREACCESS_STREAMING,
                                width,
                                height);
    backbuffer.memory = ArenaPush(arena, width * height * bytes_per_pixel);
    backbuffer.width  = width;
    backbuffer.height = height;
    backbuffer.pitch = width * bytes_per_pixel;
    backbuffer.bytes_per_pixel = bytes_per_pixel;
}

void SDLRenderBufferToWindow(SDL_Renderer *renderer){
    SDL_UpdateTexture(backbuffer.texture, 0, backbuffer.memory, backbuffer.pitch);
    SDL_RenderTexture(renderer, backbuffer.texture, 0, 0);
    SDL_RenderPresent(renderer);
}

void RenderRGBXBitmap(i32 pos_x, i32 pos_y, u32 *memory, i32 width, i32 height){
    u32 *pixels = backbuffer.memory;
    for (i32 i = 0; i < width; i++) {
        for (i32 j = 0; j < height; j++) {
            if (i + pos_x < 0 || i + pos_x > backbuffer.width ||
                j + pos_y < 0 || j + pos_y > backbuffer.height) {
                continue;
            }
            pixels[i + pos_x + (j + pos_y) * backbuffer.width] =
                memory[i + j * width];
        }
    }
}

void RenderARGBBitmap(i32 pos_x, i32 pos_y, u32 *memory, i32 width, i32 height){
    u32 *pixels = backbuffer.memory;
    for (i32 i = 0; i < width; i++) {
        for (i32 j = 0; j < height; j++) {
            if (i + pos_x < 0 || i + pos_x > backbuffer.width ||
                j + pos_y < 0 || j + pos_y > backbuffer.height) {
                continue;
            }
            pixels[i + pos_x + (j + pos_y) * backbuffer.width] = 
                BlendRGBXARGB((ColorRGBX) {.pixel = pixels[i + pos_x + (j + pos_y) * backbuffer.width]},
                              (ColorARGB) {.pixel = memory[i + j * width]}).pixel;
        }
    }
}

void FlushBuffer() {
    u32 *pixel = backbuffer.memory;
    for (i32 i = 0; i < backbuffer.width; i++) {
        for (i32 j = 0; j < backbuffer.height; j++) {
            pixel[i + j * backbuffer.width] = 0;
        }
    }
}
#endif
