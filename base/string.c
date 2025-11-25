#ifndef STRING_C
#define STRING_C
#include "base.h"
#include <unistd.h>

void PrintString(String8 string)
{
    write(1, string.string, string.size);
    fsync(1);
}

u64 c_string_len(char *str){
    u64 len = 0;
    while (str[len] != 0){
        len++;
    }
    return len;
}

String8 MakeString(Arena *arena, char *s)
{
    String8 string;
    string.size = c_string_len(s);
    string.string = ArenaPushZero(arena, string.size);
    for (u32 i = 0; s[i] != '\0'; i++)
    {
        string.string[i] = s[i];
    }
    return string;
}

String8 SubString(String8 string, u32 offset)
{
    if (offset > string.size)
    {
        return string;
    }
    String8 sub_string = {.string = string.string + offset, .size = string.size - offset};
    return sub_string;
}

String8 GetWord(String8 string)
{
    if (string.size == 0)
        return string;
    if (string.string[0] == ' ' || string.string[0] == '\n')
    {
        String8 word = {.string = string.string, .size = 1};
        return word;
    }
    for (u32 c = 0; c < string.size; c++)
    {
        switch (string.string[c])
        {
            case ' ':
            case '\n':
                {
                    String8 word = string;
                    word.size = c;
                    return word;
                }
            default:
                break;
        }
    }
    return string;
}
#endif
