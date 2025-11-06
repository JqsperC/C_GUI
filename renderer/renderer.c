#ifndef RENDERER_C
#define RENDERER_C


#include "renderer.h"

static sdl_offscreen_buffer buffer;

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

ColorRGBX RemoveAlpha(ColorRGBX background, ColorARGB foreground) {
    f32 fg = foreground.alpha / 255.0;
    f32 bg = 1 - fg;
    ColorRGBX res = {
        .red = background.red * bg + foreground.red * fg,
        .green = background.green * bg + foreground.green * fg,
        .blue = background.blue * bg + foreground.blue * fg,
    };
    return res;
}

void RenderRectangle(i32 pos_x, i32 pos_y, i32 width, i32 height, ColorRGBX color){
    ColorRGBX *pixel = (ColorRGBX*) buffer.memory;
    for (i32 x = 0; x < width && x + pos_x < buffer.width; x++){
        for (i32 y = 0; y < height && y + pos_y < buffer.height; y++){
            pixel[(y + pos_y) * buffer.width + x + pos_x] = color;
        }
    }
}

void DestroyBackbuffer(){
    if (buffer.texture){
        SDL_DestroyTexture(buffer.texture);
    }
}

void RenderRect(Rectangle rect, ColorRGBX color){
    ColorRGBX *pixel = (ColorRGBX*) buffer.memory;
    for (i32 x = 0; x < rect.width && x + rect.x < buffer.width; x++){
        for (i32 y = 0; y < rect.height && y + rect.y < buffer.height; y++){
            pixel[(y + rect.y) * buffer.width + x + rect.x] = color;
        }
    }
}

void SDLResizeBuffer(Arena *arena, SDL_Renderer *renderer, i32 width, i32 height){
    i32 bytes_per_pixel = 4;
    if (buffer.texture){
        SDL_DestroyTexture(buffer.texture);
    }
    buffer.texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_RGBX8888, 
                                SDL_TEXTUREACCESS_STREAMING,
                                width,
                                height);
    buffer.memory = ArenaPush(arena, width * height * bytes_per_pixel);
    buffer.width  = width;
    buffer.height = height;
    buffer.pitch = width * bytes_per_pixel;
    buffer.bytes_per_pixel = bytes_per_pixel;
}

void SDLRenderBufferToWindow(SDL_Renderer *renderer){
    SDL_UpdateTexture(buffer.texture, 0, buffer.memory, buffer.pitch);
    SDL_RenderTexture(renderer, buffer.texture, 0, 0);
    SDL_RenderPresent(renderer);
}


void SDLRenderARGBText(i32 offset_x, i32 offset_y, i32 height, SDL_Surface *text){
    ColorRGBX *pixel_dest;
    ColorARGB *pixel_source;
    for (i32 x = 0; x < text->w && offset_x + x < buffer.width; x++){
        for (i32 y = 0; y < text->h && offset_y + y < buffer.height && y < height; y++){

            i32 offset_dest = (offset_y + y) * buffer.pitch + (offset_x + x) * buffer.bytes_per_pixel;
            i32 offset_source = (y * text->pitch + (x * (text->pitch / text->w)));

            pixel_dest = buffer.memory + offset_dest;
            pixel_source = text->pixels + offset_source;
            *pixel_dest = RemoveAlpha(*pixel_dest, *pixel_source);
        }
    }
}

void SDLRenderText(const u8 *text,
        TTF_Font *font,
        SDL_Color color,
        i32 pos_x,
        i32 pos_y,
        i32 width,
        i32 height){
    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text, 0, color, width);
    SDLRenderARGBText(pos_x, pos_y, height, surface);
    SDL_DestroySurface(surface);
}
void FlushBuffer(){
    for (i32 i = 0; i < buffer.pitch * buffer.height; i++){
        *(u8 *)(buffer.memory + i) = 0;
    }
}

#endif
