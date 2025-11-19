#ifndef TEXT_C
#define TEXT_C

#include <ft2build.h>
#include "../base/base.h"
#include "renderer.h"

#include FT_FREETYPE_H

static Arena *bitmap_arena;

static FT_Library library;
static FT_Face face;

b32 InitializeTextRenderer(){
    i32 error = FT_Init_FreeType(&library);
    if (error) {
        DEBUG_LOG("Error Initializing freetype: %d\n", error);
        return false;
    }
    bitmap_arena = ArenaAllocate();
    if (!bitmap_arena) {
        DEBUG_LOG("Error allocating font bitmap arena for\n");
    }
    DEBUG_LOG("Freetype initialized\n");
    return true;
}

b32 LoadFont(char *filename, i32 size){
    i32 error = FT_New_Face(library, filename, 0, &face);
    if (error == FT_Err_Unknown_File_Format){
        DEBUG_LOG("Error loading font %s: Unknown file format\n", filename);
        return false;
    } else if (error){
        DEBUG_LOG("Error loadin font: %d\n", error);
        return false;
    }
    error = FT_Set_Pixel_Sizes(face, 0, size);
    if (error) {
        DEBUG_LOG("Error setting font size: %d\n", error);
        return false;
    }
    for (u8 c = 0; c < 128; c++) {
        i32 glyph_index = FT_Get_Char_Index(face, c);
        i32 error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        if (error) {
            DEBUG_LOG("Error loading glyph for character %c: %d\n", c, error);
            return false;
        }
        if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP){
            DEBUG_LOG("Rendering glyph\n");
            error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            if (error) {
                DEBUG_LOG("Error rendering glyph: %d", error);
                return false;
            }
    }
    u32 w = face->glyph->bitmap.width;
    u32 h = face->glyph->bitmap.rows;
    DEBUG_LOG("Dimensions for Character '%c': %d %d\n", c, w, h);
    }
    return true;
}

b32 RenderCharacter(i32 pos_x, i32 pos_y, u8 character, ColorRGBX color){
    i32 glyph_index = FT_Get_Char_Index(face, character);
    i32 error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    if (error) {
        DEBUG_LOG("Error loading glyph for character %c: %d\n", character, error);
        return false;
    }
    if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP){
        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if (error) {
            DEBUG_LOG("Error rendering glyph: %d", error);
            return false;
        }
    }
    u32 w = face->glyph->bitmap.width;
    u32 h = face->glyph->bitmap.rows;
    u32 *pixel = ArenaPush(bitmap_arena, h * w * sizeof(ColorRGBX));
    for (i32 i = 0; (u32) i < w; i++) {
        for (i32 j = 0; (u32) j < h; j++) {
            u8 grayscale = face->glyph->bitmap.buffer[i + j * w];
            ColorARGB fg = {.red = color.red, .green = color.green, .blue = color.blue, .alpha = grayscale};
            u32 pos = i + j * w;
            pixel[pos] = fg.pixel;
        }
    }
    RenderARGBBitmap(pos_x, pos_y, pixel, w, h);
    ArenaClear(bitmap_arena);
    return true;
}

#endif
