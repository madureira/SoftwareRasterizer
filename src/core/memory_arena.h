#ifndef CORE_MEMORY_ARENA_H
#define CORE_MEMORY_ARENA_H

#define MEMORY_ARENA_DEFAULT_ALIGNMENT ((size_t)16)

typedef struct MemoryArena
{
    uint8* base;
    size_t capacity;
    size_t used;
    size_t peak;
} MemoryArena;

typedef size_t MemoryArenaMarker;

bool memory_arena_create(MemoryArena* arena, size_t capacity);

void memory_arena_destroy(MemoryArena* arena);

void* memory_arena_alloc(MemoryArena* arena, size_t size, size_t alignment);

void* memory_arena_alloc_zero(MemoryArena* arena, size_t size, size_t alignment);

void* memory_arena_alloc_array(MemoryArena* arena, size_t count, size_t element_size,
                               size_t alignment);

void* memory_arena_alloc_array_zero(MemoryArena* arena, size_t count, size_t element_size,
                                    size_t alignment);

void memory_arena_reset(MemoryArena* arena);

bool memory_arena_get_marker(const MemoryArena* arena, MemoryArenaMarker* out_marker);

bool memory_arena_rewind(MemoryArena* arena, MemoryArenaMarker marker);

size_t memory_arena_remaining(const MemoryArena* arena);

// clang-format off
#define ARENA_PUSH_STRUCT(arena, type)              \
    ((type*)memory_arena_alloc(                     \
        (arena),                                    \
        sizeof(type),                               \
        MEMORY_ARENA_DEFAULT_ALIGNMENT))

#define ARENA_PUSH_STRUCT_ZERO(arena, type)         \
    ((type*)memory_arena_alloc_zero(                \
        (arena),                                    \
        sizeof(type),                               \
        MEMORY_ARENA_DEFAULT_ALIGNMENT))

#define ARENA_PUSH_ARRAY(arena, count, type)        \
    ((type*)memory_arena_alloc_array(               \
        (arena),                                    \
        (count),                                    \
        sizeof(type),                               \
        MEMORY_ARENA_DEFAULT_ALIGNMENT))

#define ARENA_PUSH_ARRAY_ZERO(arena, count, type)   \
    ((type*)memory_arena_alloc_array_zero(          \
        (arena),                                    \
        (count),                                    \
        sizeof(type),                               \
        MEMORY_ARENA_DEFAULT_ALIGNMENT))
// clang-format on

#endif // CORE_MEMORY_ARENA_H
