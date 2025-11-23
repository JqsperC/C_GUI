#ifndef HASH_C
#define HASH_C
#include "base.h"


u64 djb2_hash(String8 string)
{
    u64 hash = 5381;
    i32 c;
    for (u32 i = 0; i < string.size; i++)
    {
        c = string.string[i];
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

u64 hash(String8 string)
{
    return djb2_hash(string);
}

#endif
