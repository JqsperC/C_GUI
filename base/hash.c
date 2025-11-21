#ifndef HASH_C
#define HASH_C
#include "base.h"


u64 djb2_hash(u8 *str)
{
    u64 hash = 5381;
    i32 c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

u64 hash(u8 *str)
{
    return djb2_hash(str);
}

#endif
