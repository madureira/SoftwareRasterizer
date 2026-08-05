#include "core/memory_arena.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#define MEMORY_ARENA_POISON_FREED 0xDD
#endif

static bool memory_arena_is_power_of_two(const usize value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

static bool memory_arena_is_valid(const MemoryArena* arena)
{
    if (arena == NULL || arena->allocation == NULL || arena->base == NULL
        || arena->used > arena->capacity)
    {
        return false;
    }

#ifndef NDEBUG
    if (arena->self != arena)
    {
        return false;
    }
#endif

    return true;
}

bool memory_arena_create(MemoryArena* arena, usize capacity)
{
    if (arena == NULL || capacity == 0)
    {
        return false;
    }

    arena->allocation = NULL;
    arena->base = NULL;
    arena->capacity = 0;
    arena->used = 0;
    arena->peak = 0;

    usize alignment_mask = MEMORY_ARENA_MAX_ALIGNMENT - 1;

    if (capacity > SIZE_MAX - alignment_mask)
    {
        return false;
    }

    usize allocation_size = capacity + alignment_mask;

    u8* allocation = malloc(allocation_size);

    if (allocation == NULL)
    {
        return false;
    }

    usize offset = (usize)(-(uptr)allocation & alignment_mask);

    arena->allocation = allocation;
    arena->base = allocation + offset;
    arena->capacity = capacity;

#ifndef NDEBUG
    assert((uptr)arena->base % MEMORY_ARENA_MAX_ALIGNMENT_VALUE == 0);
    arena->self = arena;
    arena->generation = 0;
#endif

    return true;
}

void memory_arena_destroy(MemoryArena* arena)
{
    if (arena == NULL)
    {
        return;
    }

    free(arena->allocation);

    arena->allocation = NULL;
    arena->base = NULL;
    arena->capacity = 0;
    arena->used = 0;
    arena->peak = 0;
#ifndef NDEBUG
    arena->self = NULL;
    arena->generation = 0;
#endif
}

void* memory_arena_alloc(MemoryArena* arena, usize size, usize alignment)
{
    if (arena == NULL || arena->base == NULL || arena->allocation == NULL || size == 0
        || !memory_arena_is_power_of_two(alignment) || alignment > MEMORY_ARENA_MAX_ALIGNMENT
        || arena->used > arena->capacity)
    {
        return NULL;
    }

    usize alignment_mask = alignment - 1;

    usize padding = (alignment - (arena->used & alignment_mask)) & alignment_mask;

    usize remaining = arena->capacity - arena->used;

    if (padding > remaining)
    {
        return NULL;
    }

    remaining -= padding;

    if (size > remaining)
    {
        return NULL;
    }

    usize offset = arena->used + padding;

    arena->used = offset + size;

    if (arena->used > arena->peak)
    {
        arena->peak = arena->used;
    }

    return arena->base + offset;
}

void* memory_arena_alloc_zero(MemoryArena* arena, usize size, usize alignment)
{
    void* memory = memory_arena_alloc(arena, size, alignment);

    if (memory != NULL)
    {
        memset(memory, 0, size);
    }

    return memory;
}

void* memory_arena_alloc_array(MemoryArena* arena, usize count, usize element_size, usize alignment)
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

    usize size = count * element_size;

    return memory_arena_alloc(arena, size, alignment);
}

void* memory_arena_alloc_array_zero(MemoryArena* arena, usize count, usize element_size,
                                    usize alignment)
{
    if (count == 0 || element_size == 0)
    {
        return NULL;
    }

    if (count > SIZE_MAX / element_size)
    {
        return NULL;
    }

    usize size = count * element_size;

    return memory_arena_alloc_zero(arena, size, alignment);
}

void memory_arena_reset(MemoryArena* arena)
{
    if (!memory_arena_is_valid(arena))
    {
        return;
    }

#ifndef NDEBUG
    memset(arena->base, MEMORY_ARENA_POISON_FREED, arena->used);
#endif

    /*
     * Do not clear the memory
     * It just allows subsequent allocations to reuse the previous space.
     */
    arena->used = 0;

#ifndef NDEBUG
    arena->generation++;
#endif
}

bool memory_arena_get_marker(const MemoryArena* arena, MemoryArenaMarker* out_marker)
{
    if (!memory_arena_is_valid(arena) || out_marker == NULL)
    {
        return false;
    }

    out_marker->used = arena->used;

#ifndef NDEBUG
    out_marker->arena = arena;
    out_marker->generation = arena->generation;
#endif

    return true;
}

bool memory_arena_rewind(MemoryArena* arena, MemoryArenaMarker marker)
{
    if (!memory_arena_is_valid(arena) || marker.used > arena->used)
    {
        return false;
    }

#ifndef NDEBUG
    if (marker.arena != arena || marker.generation != arena->generation)
    {
        return false;
    }

    memset(arena->base + marker.used, MEMORY_ARENA_POISON_FREED, arena->used - marker.used);
#endif

    arena->used = marker.used;

    return true;
}

usize memory_arena_remaining(const MemoryArena* arena)
{
    if (!memory_arena_is_valid(arena))
    {
        return 0;
    }

    return arena->capacity - arena->used;
}

usize memory_arena_remaining_aligned(const MemoryArena* arena, usize alignment)
{
    if (arena == NULL || arena->allocation == NULL || arena->base == NULL
        || arena->used > arena->capacity || !memory_arena_is_power_of_two(alignment)
        || alignment > MEMORY_ARENA_MAX_ALIGNMENT)
    {
        return 0;
    }

    usize mask = alignment - 1;

    usize padding = (alignment - (arena->used & mask)) & mask;

    usize remaining = arena->capacity - arena->used;

    if (padding > remaining)
    {
        return 0;
    }

    return remaining - padding;
}
