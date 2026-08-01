#include "core/memory_arena.h"

#include <stdlib.h>
#include <string.h>

static bool memory_arena_is_power_of_two(size_t value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

bool memory_arena_create(MemoryArena* arena, size_t capacity)
{
    if (arena == NULL || capacity == 0)
    {
        return false;
    }

    arena->base = NULL;
    arena->capacity = 0;
    arena->used = 0;
    arena->peak = 0;

    size_t actual = capacity + MEMORY_ARENA_DEFAULT_ALIGNMENT - 1;

    uint8* memory = (uint8*)malloc(actual);

    if (memory == NULL)
    {
        return false;
    }

    arena->base = memory;
    arena->capacity = actual;

    return true;
}

void memory_arena_destroy(MemoryArena* arena)
{
    if (arena == NULL)
    {
        return;
    }

    free(arena->base);

    arena->base = NULL;
    arena->capacity = 0;
    arena->used = 0;
    arena->peak = 0;
}

void* memory_arena_alloc(MemoryArena* arena, size_t size, size_t alignment)
{
    if (arena == NULL || arena->base == NULL || size == 0
        || !memory_arena_is_power_of_two(alignment) || arena->used > arena->capacity)
    {
        return NULL;
    }

    uintptr_t current_address = (uintptr_t)(arena->base + arena->used);

    uintptr_t alignment_mask = (uintptr_t)(alignment - 1);

    /*
     * Avoid overlow on:
     * current_address + alignment_mask
     */
    if (current_address > UINTPTR_MAX - alignment_mask)
    {
        return NULL;
    }

    uintptr_t aligned_address = (current_address + alignment_mask) & ~alignment_mask;

    size_t padding = (size_t)(aligned_address - current_address);

    size_t remaining = arena->capacity - arena->used;

    if (padding > remaining)
    {
        return NULL;
    }

    remaining -= padding;

    if (size > remaining)
    {
        return NULL;
    }

    arena->used += padding + size;

    if (arena->used > arena->peak)
    {
        arena->peak = arena->used;
    }

    return (void*)aligned_address;
}

void* memory_arena_alloc_zero(MemoryArena* arena, size_t size, size_t alignment)
{
    void* memory = memory_arena_alloc(arena, size, alignment);

    if (memory != NULL)
    {
        memset(memory, 0, size);
    }

    return memory;
}

void* memory_arena_alloc_array(MemoryArena* arena, size_t count, size_t element_size,
                               size_t alignment)
{
    if (count == 0 || element_size == 0)
    {
        return NULL;
    }

    /*
     * Avoid overflow on:
     * count * element_size
     */
    if (count > SIZE_MAX / element_size)
    {
        return NULL;
    }

    size_t size = count * element_size;

    return memory_arena_alloc(arena, size, alignment);
}

void* memory_arena_alloc_array_zero(MemoryArena* arena, size_t count, size_t element_size,
                                    size_t alignment)
{
    if (count == 0 || element_size == 0)
    {
        return NULL;
    }

    if (count > SIZE_MAX / element_size)
    {
        return NULL;
    }

    size_t size = count * element_size;

    return memory_arena_alloc_zero(arena, size, alignment);
}

void memory_arena_reset(MemoryArena* arena)
{
    if (arena == NULL)
    {
        return;
    }

    /*
     * Do not clear the memory
     *
     * It just allows subsequent allocations to reuse the previous space.
     */
    arena->used = 0;
}

bool memory_arena_get_marker(const MemoryArena* arena, MemoryArenaMarker* out_marker)
{
    if (arena == NULL || out_marker == NULL)
    {
        return false;
    }

    *out_marker = arena->used;

    return true;
}

bool memory_arena_rewind(MemoryArena* arena, MemoryArenaMarker marker)
{
    if (arena == NULL || marker > arena->used)
    {
        return false;
    }

    arena->used = marker;

    return true;
}

size_t memory_arena_remaining(const MemoryArena* arena)
{
    if (arena == NULL || arena->used > arena->capacity)
    {
        return 0;
    }

    return arena->capacity - arena->used;
}
