#ifndef TEXT_C
#define TEXT_C

#include <ft2build.h>
#include "../base/base.h"
#include "renderer.h"

#include FT_FREETYPE_H

static Arena *bitmap_arena;
static Arena *render_arena;
static Arena *text_line_arena;

static glyph glyphs[ASCII_NUM_GLYPHS];

static i32 max_character_height = 0;
static i32 line_spacing = 0;

static FT_Library library;
static FT_Face face;

typedef struct TextLine
{
    struct TextLine *next;
    u32 width_pixels;
    String8 text;

} TextLine;

typedef enum 
{
    AXIS_X = 0,
    AXIS_Y,
    AXIS_COUNT
} axis;

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
    text_line_arena = ArenaAllocate();
    if (!text_line_arena) {
        DEBUG_LOG("Error allocating text line arena for text renderer\n");
    }
    render_arena = ArenaAllocate();
    if (!render_arena) {
        DEBUG_LOG("Error allocating render arena for text renderer\n");
    }
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
    u32 *pixel = ArenaPush(render_arena, w * h);
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
    ArenaClear(render_arena);
    return glyph.advance;
}

i32 GetCharacterRenderWidth(u8 character)
{
    glyph glyph = glyphs[character];
    return glyph.advance;
}

i32 GetLineWidthNoWrap(String8 string)
{
    u32 width = 0;
    for (u32 i = 0; i < string.size; i++){
        if (string.string[i] == '\n')
        {
            return width;
        }
        glyph glyph = glyphs[string.string[i]];
        width += glyph.advance;
    }
    return width;
}

i32 GetNextWordRenderWidth(String8 string)
{ 
    u32 width = 0;
    String8 word = GetWord(string);
    for (u32 c = 0; c < string.size; c++){
        glyph glyph = glyphs[word.string[c]];
        width += glyph.advance;
    }
    return width;
}

TextLine *SplitTextIntoLines(Arena *result, String8 string, text_wrap_kind wrap, u32 max_line_width)
{
    TextLine *current_line = ArenaPushZero(result, sizeof(TextLine));
    TextLine *first = current_line;
    first->text = string;
    switch (wrap)
    {
        case WRAP_KIND_CHAR:
        {
            u32 line_width_pixels = 0;
            u32 line_num_chars = 0;
            for (u32 c = 0; c < string.size; c++)
            {
                u8 character = string.string[c];
                u32 w = GetCharacterRenderWidth(character);
                if (line_width_pixels + w > max_line_width || character == '\n')
                {
                    TextLine *line = ArenaPushZero(result, sizeof(TextLine));
                    current_line->text.size = line_num_chars;
                    current_line->width_pixels = line_width_pixels;
                    current_line->next = line;
                    current_line = line;
                    current_line->text = SubString(string, c);
                    line_width_pixels = 0;
                    line_num_chars = 0;
                }
                if ((character == ' ' && line_num_chars == 0) ||
                    character == '\n')
                {
                    current_line->text = SubString(string, c + 1);
                }
                line_num_chars++;
                line_width_pixels += w;
            }
            current_line->text.size = line_num_chars;
            current_line->width_pixels = line_width_pixels;
            break;
        }

        case WRAP_KIND_WORD:
        {
            u32 line_width_pixels = 0;
            u32 line_num_chars = 0;
            u32 c = 0;
            while (c < string.size)
            {
                u8 character = string.string[c];
                String8 current_substring = SubString(string, c);
                String8 current_word = GetWord(current_substring);
                u32 w = GetNextWordRenderWidth(current_word);
                if (line_width_pixels + w > max_line_width || character == '\n')
                {
                    TextLine *line = ArenaPushZero(result, sizeof(TextLine));
                    current_line->text.size = line_num_chars;
                    current_line->width_pixels = line_width_pixels;
                    current_line->next = line;
                    current_line = line;
                    current_line->text = SubString(string, c);
                    line_width_pixels = 0;
                    line_num_chars = 0;
                }
                if ((character == ' ' && line_num_chars == 0) ||
                    character == '\n')
                {
                    current_line->text = SubString(string, c + 1);
                } else {
                    line_num_chars += current_word.size;
                    line_width_pixels += w;
                }
                c += current_word.size;
            }
            current_line->text.size = line_num_chars;
            current_line->width_pixels = line_width_pixels;
            break;
        }
        case WRAP_KIND_NONE:
        default:
        {
            break;
        }
    }
    return first;
   }

void RenderTextRect(Rectangle rect, String8 text, ColorRGBX color, text_wrap_kind wrap, text_horizontal_align_kind align_x, text_vertical_align_kind align_y)
{
    RenderText(rect.x, rect.y, rect.width, rect.height, text, color, wrap, align_x, align_y);
}

void RenderText(i32 pos_x, i32 pos_y, i32 width, i32 height, String8 text, ColorRGBX color, text_wrap_kind wrap, text_horizontal_align_kind align_x, text_vertical_align_kind align_y)
{
    if (!text.string) {
        return;
    }
    i32 cursor_x = 0;
    i32 cursor_y = 0;

    TextLine *line = SplitTextIntoLines(text_line_arena, text, wrap, width);

    u32 num_lines = 0;

    for (TextLine *current = line; current != NULL; current = current->next)
    {
        num_lines++;
    }

    u32 y_align_offset = 0;
    i32 text_height = num_lines * line_spacing;
    if (text_height < height) {
        switch(align_y)
        {
            case VERTICAL_ALIGN_CENTER:
                y_align_offset = (height - text_height) / 2.0f;
                break;
            case VERTICAL_ALIGN_BOTTOM:
                y_align_offset = height - text_height;
                break;
            default:
                y_align_offset = 0; 
        }
    }
    cursor_y = y_align_offset;
    while (line != NULL)
    {
        String8 string = line->text;
        u32 align_offset = 0;
        if (line->width_pixels < (u32)width){
            switch (align_x)
            {
                case HORIZONTAL_ALIGN_CENTER:
                    {
                        align_offset = (width - line->width_pixels) / 2.0f;
                        break;
                    }
                case HORIZONTAL_ALIGN_RIGHT:
                    {
                        align_offset = width - line->width_pixels;
                        break;
                    }
                default:
                    {
                        align_offset = 0;
                    }
            }
        }
        cursor_x = align_offset;
        for (u32 c = 0; c < string.size; c++)
        {
            u8 character = string.string[c];
            i32 bearing = RenderCharacter(pos_x + cursor_x, pos_y + cursor_y, character, color);
            cursor_x += bearing;
        }
        cursor_y += line_spacing;
        if (cursor_y + max_character_height > height)
        {
            break;
        }
       line = line->next;
    }

    ArenaClear(text_line_arena);
}

#endif
