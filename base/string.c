#ifndef STRING_C
#define STRING_C
#include "base.h"

u64 c_string_len(u8 *str){
    u64 len = 0;
    while (str[len] != 0){
        len++;
    }
    return len;
}

#endif
