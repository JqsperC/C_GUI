#ifndef MATH_C
#define MATH_C

#include "base.h"

#define PI 3.14159265358979f


i32 min(i32 a, i32 b) {
    return a < b ? a : b;
}

i32 max(i32 a, i32 b){
    return a > b ? a : b;
}

b32 IsPointInRectangle(i32 x, i32 y, Rectangle rect) {
    return(x >= rect.x && x <= rect.x + rect.width && y >= rect.y && y <= rect.y + rect.height);
}

#endif
