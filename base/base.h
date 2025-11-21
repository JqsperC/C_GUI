#ifndef BASE_H
#define BASE_H

#include <stdint.h>
#include <stdio.h>

#define ArrayCount(Array) (sizeof(Array)/(sizeof((Array)[0])))
#define Kilobytes(Value) ((u64)(Value) * 1024)
#define Megabytes(Value) ((u64)(Value) * 1024 * 1024)
#define Gigabytes(Value) ((u64)(Value) * 1024 * 1024 * 1024)

#ifdef DEBUG
#define DEBUG_LOG(...) printf(__VA_ARGS__)
#else
#define DEBUG_LOG(fmt, ...) 
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef i32 b32;

/* ARENA */
#define ARENA_STANDARD_SIZE Megabytes(2)
typedef struct _arena {
    void *memory_base;
    u64 offset;
    u64 capacity;
} Arena;

Arena *ArenaAllocate();
Arena *ArenaAllocateFixedSize(u64 size);
void ArenaRelease(Arena*);

void *ArenaPush(Arena*, u64);
void *ArenaPushZero(Arena*, u64);

#define PushArray(arena, type, count) (type*)ArenaPush((arena), sizeof(type) * (count))
#define PushArrayZero(arena, type, count) (type*)ArenaPushZero((arena), sizeof(type) * (count))
#define PushStruct(arena, type) PushArray((arena), (type), 1)
#define PushStructZero(arena, type) PushArrayZero((arena), (type), 1)

u64 ArenaGetPos(Arena*);
void ArenaPop(Arena*, u64);
void ArenaClear(Arena*);
void ArenaClearZero(Arena*);

/* Pool Alocator */


typedef struct PoolNode PoolNode;
struct PoolNode {
    struct PoolNode *next;
};

typedef struct Pool Pool;
struct Pool{
    u8 *buffer;
    u64 buffer_len;
    u64 chunk_size;

    PoolNode *head;
};

void PoolInit(Pool *p, void *backing_buffer, u32 backing_buffer_length,u32 chunk_size);
void PoolClear(Pool *p);

void *PoolAlloc(Pool *p);
void PoolFree(Pool *p, void *ptr);

/* STRINGS */
typedef struct _string8 {
    u32 size;
    u8 *string;
} String8;

u64 c_string_len(u8*);
/* MATH*/

typedef union {
    struct {
        f32 x;
        f32 y;
    };
    f32 v[2];
} v2f32;


typedef struct {
    i32 x;
    i32 y;
    i32 width;
    i32 height;
} Rectangle;

i32 min(i32, i32);
i32 max(i32, i32);

b32 IsPointInRectangle(i32, i32, Rectangle);

/*HASH (TABLE?)*/
/* djb2 hash function */
u64 hash(u8 *str);

#endif
