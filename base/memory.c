#ifndef MEMORY_C
#define MEMORY_C

#include <stdlib.h>

#include "base.h"

Arena *ArenaAllocate(){
    Arena *arena = malloc(sizeof(Arena));
    arena->capacity = ARENA_STANDARD_SIZE;
    arena->memory_base = malloc(ARENA_STANDARD_SIZE);
    arena->offset = 0;
    if (arena->memory_base == NULL){
        return NULL;
    }
    return arena;
}
void ArenaRelease(Arena *arena){
    if (arena){
        free(arena->memory_base);
    }
    free(arena);
}

void *ArenaPush(Arena *arena, u64 size){
    void *memory;
    if (arena->capacity - arena->offset >= size) {
        memory = arena->memory_base + arena->offset;
        arena->offset += size;
        return memory;
    }
    return NULL;
}
void *ArenaPushZero(Arena *arena, u64 size){
    void *memory = ArenaPush(arena, size);
    for (int i = 0; i < size; i++) {
        *(((char*)memory)+i) = 0;
    }
    return memory;
}

#define PushArray(arena, type, count) (type*)ArenaPush((arena), sizeof(type) * (count))
#define PushArrayZero(arena, type, count) (type*)ArenaPushZero((arena), sizeof(type) * (count))
#define PushStruct(arena, type) PushArray((arena), (type), 1)
#define PushStructZero(arena, type) PushArrayZero((arena), (type), 1)

u64 ArenaGetPos(Arena *arena){
    return arena->offset;
}

void ArenaPop(Arena *arena, u64 size){
    if (arena->offset > size) {
        arena->offset -= size;
    } else {
        arena->offset = 0;
    }
}

void ArenaClear(Arena *arena){
    arena->offset = 0;
}

void ArenaClearZero(Arena *arena){
    arena->offset = 0;
    for (int i = 0; i < arena->capacity; i++){
        *((u8*)(arena->memory_base)+i) = 0;
    }
}


#endif
