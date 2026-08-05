#ifndef CORE_MEMORY_ARENA_H
#define CORE_MEMORY_ARENA_H

#define MEM_ARENA_INIT                       { 0 }
#define MEMORY_ARENA_MAX_ALIGNMENT_VALUE     64
#define MEMORY_ARENA_DEFAULT_ALIGNMENT_VALUE 16

#if MEMORY_ARENA_MAX_ALIGNMENT_VALUE == 0                                                          \
    || (MEMORY_ARENA_MAX_ALIGNMENT_VALUE & (MEMORY_ARENA_MAX_ALIGNMENT_VALUE - 1)) != 0
#error "MEMORY_ARENA_MAX_ALIGNMENT_VALUE must be a power of two."
#endif

#define MEMORY_ARENA_MAX_ALIGNMENT     ((usize)MEMORY_ARENA_MAX_ALIGNMENT_VALUE)
#define MEMORY_ARENA_DEFAULT_ALIGNMENT ((usize)MEMORY_ARENA_DEFAULT_ALIGNMENT_VALUE)

#if defined(_MSC_VER)
#define MEMORY_ALIGNOF(type) ((usize) __alignof(type))
#elif defined(__clang__) || defined(__GNUC__)
#define MEMORY_ALIGNOF(type) ((usize) __alignof__(type))
#else
#define MEMORY_ALIGNOF(type) MEMORY_ARENA_DEFAULT_ALIGNMENT
#endif

typedef struct MemoryArena
{
    u8* allocation;
    u8* base;
    usize capacity;
    usize used;
    usize peak;

#ifndef NDEBUG
    const struct MemoryArena* self;
    u64 generation;
#endif
} MemoryArena;

typedef struct MemoryArenaMarker
{
    usize used;

#ifndef NDEBUG
    const MemoryArena* arena;
    u64 generation;
#endif
} MemoryArenaMarker;

bool memory_arena_create(MemoryArena* arena, usize capacity);

void memory_arena_destroy(MemoryArena* arena);

void* memory_arena_alloc(MemoryArena* arena, usize size, usize alignment);

void* memory_arena_alloc_zero(MemoryArena* arena, usize size, usize alignment);

void* memory_arena_alloc_array(MemoryArena* arena, usize count, usize element_size,
                               usize alignment);

void* memory_arena_alloc_array_zero(MemoryArena* arena, usize count, usize element_size,
                                    usize alignment);

void memory_arena_reset(MemoryArena* arena);

bool memory_arena_get_marker(const MemoryArena* arena, MemoryArenaMarker* out_marker);

bool memory_arena_rewind(MemoryArena* arena, MemoryArenaMarker marker);

usize memory_arena_remaining(const MemoryArena* arena);

usize memory_arena_remaining_aligned(const MemoryArena* arena, usize alignment);

#define MEM_ARENA_PUSH_STRUCT(arena, type)                                                         \
    ((type*)memory_arena_alloc((arena), sizeof(type), MEMORY_ALIGNOF(type)))

#define MEM_ARENA_PUSH_STRUCT_ZERO(arena, type)                                                    \
    ((type*)memory_arena_alloc_zero((arena), sizeof(type), MEMORY_ALIGNOF(type)))

#define MEM_ARENA_PUSH_ARRAY(arena, count, type)                                                   \
    ((type*)memory_arena_alloc_array((arena), (count), sizeof(type), MEMORY_ALIGNOF(type)))

#define MEM_ARENA_PUSH_ARRAY_ZERO(arena, count, type)                                              \
    ((type*)memory_arena_alloc_array_zero((arena), (count), sizeof(type), MEMORY_ALIGNOF(type)))

#endif // CORE_MEMORY_ARENA_H
