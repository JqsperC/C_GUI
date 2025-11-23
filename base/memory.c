#ifndef MEMORY_C
#define MEMORY_C

#include <stdlib.h>

#include "base.h"

void ZeroMemory(void *mem, u32 n)
{
    u8 *memory = mem;
    for (u32 i = 0; i < n; i++)
    {
        memory[i] = 0;
    }
}

Arena *ArenaAllocate(){
    Arena *arena = malloc(sizeof(Arena));
    arena->capacity = ARENA_STANDARD_SIZE;
    arena->memory_base = malloc(ARENA_STANDARD_SIZE);
    arena->offset = 0;
    if (arena->memory_base == NULL){
        DEBUG_LOG("Failed to Allocate Arena\n");
        return NULL;
    }
    return arena;
}

Arena *ArenaAllocateFixedSize(u64 size){
    Arena *arena = malloc(sizeof(Arena));
    arena->capacity = size;
    arena->memory_base = malloc(size);
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
    DEBUG_LOG("Allocation Failed: No more memory in Arena\n");
    return NULL;
}
void *ArenaPushZero(Arena *arena, u64 size){
    void *memory = ArenaPush(arena, size);
    if (memory)
    {
        ZeroMemory(memory, size);\
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
    for (u64 i = 0; i < arena->capacity; i++){
        *((u8*)(arena->memory_base)+i) = 0;
    }
}


void *PoolAlloc(Pool *p)
{
    PoolNode *node = p->head;
    p->head = p->head->next;
    ZeroMemory(node, p->chunk_size);
    return node;
}


void PoolFree(Pool *p, void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    void *start = p->buffer;
    void *end = &p->buffer[p->buffer_len];
    if (ptr < start ||ptr > end)
    {
        DEBUG_LOG("Memory out of bounds for pool");
        return;
    }
    PoolNode *node = ptr;
    node->next = p->head;
    p->head = node;
}

void PoolClear(Pool *p)
{
    u32 num_chunks = p->buffer_len / p->chunk_size;
    for (u32 i = 0; i < num_chunks; i++)
    {
        void *ptr = &p->buffer[i * p->chunk_size];
        PoolNode *node = ptr;
        node->next = p->head;
        p->head = node;
    }
}

void PoolInit(Pool *p, void *backing_buffer, u32 backing_buffer_length,u32 chunk_size)
{
    p->chunk_size = chunk_size;
    p->buffer_len = backing_buffer_length;
    p->buffer = backing_buffer;
    p->head = NULL;
    PoolClear(p);
}
#endif
