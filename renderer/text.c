#ifndef TEXT_C
#define TEXT_C

#include <ft2build.h>
#include "../base/base.h"
#include "renderer.h"

#include FT_FREETYPE_H

static Arena *bitmap_arena;
static Arena *scratch;
static glyph glyphs[ASCII_NUM_GLYPHS];

static i32 max_character_height = 0;
static i32 line_spacing = 0;

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
    scratch = ArenaAllocate();
    if (!scratch) {
        DEBUG_LOG("Error allocating scratch arena for text renderer\n");
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
    if (error) 
    {
        DEBUG_LOG("Error setting font size: %d\n", error);
        return false;
    }
    line_spacing = face->size->metrics.height / 64.0f;
    for (u8 c = 0; c < 128; c++) 
    {
        i32 glyph_index = FT_Get_Char_Index(face, c);
        i32 error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
        if (error) {
            DEBUG_LOG("Error loading glyph for character %c: %d\n", c, error);
            return false;
        }
        if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
        {
            DEBUG_LOG("Rendering glyph\n");
            error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            if (error)
            {
                DEBUG_LOG("Error rendering glyph: %d", error);
                return false;
            }
        }
        glyph glyph;
        glyph.advance = face->glyph->advance.x / 64.0f;
        glyph.bitmap_width = face->glyph->bitmap.width;
        glyph.bitmap_height = face->glyph->bitmap.rows;
        glyph.bearing_x = face->glyph->bitmap_left;
        glyph.bearing_y = face->glyph->bitmap_top;
        glyph.bitmap = ArenaPush(bitmap_arena, glyph.bitmap_width * glyph.bitmap_height);
        for (i32 i = 0; i < glyph.bitmap_width * glyph.bitmap_height; i ++) {
            glyph.bitmap[i] = face->glyph->bitmap.buffer[i];
        }
        glyphs[c] = glyph;
        if (glyph.bearing_y > max_character_height)
        {
            max_character_height = glyph.bearing_y;
        }
    }
    return true;
}

i32 RenderCharacter(i32 cursor_x, i32 cursor_y, u8 character, ColorRGBX color)
{
    glyph glyph = glyphs[character];
    i32 w = glyph.bitmap_width;
    i32 h = glyph.bitmap_height;
    u32 *pixel = ArenaPush(scratch, w * h);
    // DEBUG_LOG("Rendering character %c, dim: %d %d \n", character, w, h);
    for (i32 i = 0; i < w; i++) 
    {
        for (i32 j = 0; j < h; j++)
        {
            u8 grayscale = glyph.bitmap[i + j * w];
            ColorARGB fg = {.red = color.red, .green = color.green, .blue = color.blue, .alpha = grayscale};
            u32 pos = i + j * w;
            pixel[pos] = fg.pixel;
        }
    }
    i32 pos_x = cursor_x + glyph.bearing_x;
    i32 pos_y = cursor_y + max_character_height - glyph.bearing_y;
    RenderARGBBitmap(pos_x, pos_y, pixel, w, h);
    ArenaClear(scratch);
    return glyph.advance;
}
i32 GetCharacterRenderWidth(u8 character)
{
    glyph glyph = glyphs[character];
    return glyph.advance;
}

i32 GetStringRenderWidth(u8 *string)
{
    u32 width = 0;
    for (u8 *c = string; *c != 0; c++){
        glyph glyph = glyphs[*c];
        width += glyph.advance;
    }
    return width;
}

i32 GetWordRenderWidth(u8 *string)
{ 
    u32 width = 0;
    for (u8 *c = string; *c != 0; c++){
        glyph glyph = glyphs[*c];
        width += glyph.advance;
        if (*c == ' ')
        {
            return width;
        }
    }
    return width;
}

i32 GetTextRenderLineWidth (u8 *string, i32 width, text_wrap_kind wrap)
{
    i32 line_width = 0;
    for (u8 *c = string; *c != 0; c++)
    {
        u8 character = *c;
        switch (wrap)
        {
            case WRAP_KIND_CHAR:
                {
                    if (GetCharacterRenderWidth(character) + line_width > width)
                    {
                        return line_width;
                    }
                }
            case WRAP_KIND_WORD:
                {
                    i32 word_width = GetWordRenderWidth(c);
                    if (word_width + line_width > width && word_width < width) {
                        return line_width;
                    }
                    break;
                }
            default:
                {
                    return width;
                }
        }
        line_width += GetCharacterRenderWidth(character);
    }
    return line_width;
}

void RenderText(i32 pos_x, i32 pos_y, i32 width, i32 height, u8 *string, ColorRGBX color, text_wrap_kind wrap, text_align_kind align)
{
    if (!string) {
        return;
    }
    i32 cursor_x = 0;
    i32 cursor_y = 0;
    
    i32 line_width = GetTextRenderLineWidth(string, width, wrap);
    for (u8 *c = string; *c != 0 && cursor_y < height ; c++)
    {
        u8 character = *c;
        switch (wrap)
        {
            case WRAP_KIND_CHAR:
                {
                    if (GetCharacterRenderWidth(character) + cursor_x > width)
                    {
                        cursor_x = 0;
                        cursor_y += line_spacing;
                        if (cursor_y + line_spacing > height) return;
                    }
                }
            case WRAP_KIND_WORD:
                {
                    i32 word_width = GetWordRenderWidth(c);
                    if (word_width + cursor_x > width && word_width < width) {
                        cursor_x = 0;
                        cursor_y += line_spacing;
                        if (cursor_y + line_spacing > height) return;
                    }
                    break;
                }
            default:
                {
                    break;
                }
        }
        if (cursor_x == 0 && *c == ' ')
        { 
            continue;
        }
        if (cursor_x == 0)
        {
            line_width = GetTextRenderLineWidth(c, width, wrap);
            switch (align) {
                case ALIGN_CENTER:
                {
                    cursor_x = (width - line_width) /  2.0f;
                    break;
                }
                case ALIGN_RIGHT:
                    {
                        cursor_x = (width - line_width);
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
        }
        i32 bearing = RenderCharacter(pos_x + cursor_x, pos_y + cursor_y, character, color);
        cursor_x += bearing;
    }
}



#endif
