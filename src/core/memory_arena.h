#ifndef CORE_MEMORY_ARENA_H
#define CORE_MEMORY_ARENA_H

#define MEMORY_ARENA_MAX_ALIGNMENT_VALUE     64
#define MEMORY_ARENA_DEFAULT_ALIGNMENT_VALUE 16

// clang-format off
#if MEMORY_ARENA_MAX_ALIGNMENT_VALUE == 0 \
    || (MEMORY_ARENA_MAX_ALIGNMENT_VALUE \
        & (MEMORY_ARENA_MAX_ALIGNMENT_VALUE - 1)) != 0
#error "MEMORY_ARENA_MAX_ALIGNMENT_VALUE must be a power of two."
#endif
// clang-format on

#define MEMORY_ARENA_MAX_ALIGNMENT     ((size_t)MEMORY_ARENA_MAX_ALIGNMENT_VALUE)
#define MEMORY_ARENA_DEFAULT_ALIGNMENT ((size_t)MEMORY_ARENA_DEFAULT_ALIGNMENT_VALUE)

#if defined(_MSC_VER)
#define MEMORY_ALIGNOF(type) ((size_t)__alignof(type))
#elif defined(__clang__) || defined(__GNUC__)
#define MEMORY_ALIGNOF(type) ((size_t)__alignof__(type))
#else
#define MEMORY_ALIGNOF(type) MEMORY_ARENA_DEFAULT_ALIGNMENT
#endif

typedef struct MemoryArena
{
    uint8* allocation;
    uint8* base;
    size_t capacity;
    size_t used;
    size_t peak;

#ifndef NDEBUG
    const struct MemoryArena* self;
    uint32 generation;
#endif
} MemoryArena;

typedef struct MemoryArenaMarker
{
    size_t used;

#ifndef NDEBUG
    const MemoryArena* arena;
    uint32 generation;
#endif
} MemoryArenaMarker;

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

size_t memory_arena_remaining_aligned(const MemoryArena* arena, size_t alignment);

// clang-format off
#define MEM_ARENA_PUSH_STRUCT(arena, type)              \
    ((type*)memory_arena_alloc(                         \
        (arena),                                        \
        sizeof(type),                                   \
        MEMORY_ALIGNOF(type)))

#define MEM_ARENA_PUSH_STRUCT_ZERO(arena, type)         \
    ((type*)memory_arena_alloc_zero(                    \
        (arena),                                        \
        sizeof(type),                                   \
        MEMORY_ALIGNOF(type)))

#define MEM_ARENA_PUSH_ARRAY(arena, count, type)        \
    ((type*)memory_arena_alloc_array(                   \
        (arena),                                        \
        (count),                                        \
        sizeof(type),                                   \
        MEMORY_ALIGNOF(type)))

#define MEM_ARENA_PUSH_ARRAY_ZERO(arena, count, type)   \
    ((type*)memory_arena_alloc_array_zero(              \
        (arena),                                        \
        (count),                                        \
        sizeof(type),                                   \
        MEMORY_ALIGNOF(type)))
// clang-format on

#endif // CORE_MEMORY_ARENA_H
