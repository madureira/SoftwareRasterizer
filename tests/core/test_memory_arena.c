#include <string.h>

#include "test.h"

#include "core/memory_arena.h"

/*
 * create
 */

static void test_create_null_arena_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, memory_arena_create(NULL, 1024));
}

static void test_create_zero_capacity_returns_false(TestContext* ctx)
{
    MemoryArena arena;
    TEST_ASSERT_FALSE(ctx, memory_arena_create(&arena, 0));
}

static void test_create_capacity_overflow_returns_false(TestContext* ctx)
{
    MemoryArena arena;
    usize overflow_capacity = SIZE_MAX - (MEMORY_ARENA_MAX_ALIGNMENT - 2);
    TEST_ASSERT_FALSE(ctx, memory_arena_create(&arena, overflow_capacity));
}

static void test_create_valid_initializes_fields(TestContext* ctx)
{
    MemoryArena arena;
    TEST_ASSERT_TRUE(ctx, memory_arena_create(&arena, 1024));
    TEST_ASSERT_NOT_NULL(ctx, arena.base);
    TEST_ASSERT(ctx, arena.capacity >= 1024);
    TEST_ASSERT_INT_EQ(ctx, (int)arena.used, 0);
    TEST_ASSERT_INT_EQ(ctx, (int)arena.peak, 0);
    memory_arena_destroy(&arena);
}

/*
 * alloc
 */

static void test_alloc_null_arena_returns_null(TestContext* ctx)
{
    TEST_ASSERT_NULL(ctx, memory_arena_alloc(NULL, 16, 16));
}

static void test_alloc_zero_size_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc(&arena, 0, 16));
    memory_arena_destroy(&arena);
}

static void test_alloc_non_power_of_two_alignment_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc(&arena, 16, 3));
    memory_arena_destroy(&arena);
}

static void test_alloc_returns_non_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NOT_NULL(ctx, memory_arena_alloc(&arena, 16, 16));
    memory_arena_destroy(&arena);
}

static void test_alloc_address_is_aligned(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);

    memory_arena_alloc(&arena, 1, 1);

    void* ptr = memory_arena_alloc(&arena, 1, 16);
    TEST_ASSERT_NOT_NULL(ctx, ptr);
    TEST_ASSERT_INT_EQ(ctx, (int)((uptr)ptr % 16), 0);

    memory_arena_destroy(&arena);
}

static void test_alloc_exhausted_arena_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 16);
    memory_arena_alloc(&arena, 16, 1);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc(&arena, 16, 1));
    memory_arena_destroy(&arena);
}

static void test_alloc_updates_used(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_alloc(&arena, 32, 1);
    TEST_ASSERT_INT_EQ(ctx, (int)arena.used, 32);
    memory_arena_destroy(&arena);
}

static void test_alloc_updates_peak(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_alloc(&arena, 64, 1);
    TEST_ASSERT_INT_EQ(ctx, (int)arena.peak, 64);
    memory_arena_destroy(&arena);
}

static void test_alloc_zero_alignment_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc(&arena, 16, 0));
    memory_arena_destroy(&arena);
}

static void test_alloc_alignment_exceeds_max_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc(&arena, 16, MEMORY_ARENA_MAX_ALIGNMENT * 2));
    memory_arena_destroy(&arena);
}

/*
 * alloc_zero
 */

static void test_alloc_zero_returns_zeroed_memory(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);

    u8* dirty = (u8*)memory_arena_alloc(&arena, 64, 1);
    if (dirty != NULL)
    {
        memset(dirty, 0xFF, 64);
    }

    memory_arena_reset(&arena);

    u8* mem = (u8*)memory_arena_alloc_zero(&arena, 64, 1);
    TEST_ASSERT_NOT_NULL(ctx, mem);

    bool all_zero = true;
    for (usize i = 0; i < 64; i++)
    {
        if (mem[i] != 0)
        {
            all_zero = false;
            break;
        }
    }
    TEST_ASSERT_TRUE(ctx, all_zero);

    memory_arena_destroy(&arena);
}

/*
 * alloc_array
 */

static void test_alloc_array_zero_count_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc_array(&arena, 0, 4, 4));
    memory_arena_destroy(&arena);
}

static void test_alloc_array_zero_element_size_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc_array(&arena, 4, 0, 4));
    memory_arena_destroy(&arena);
}

static void test_alloc_array_overflow_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc_array(&arena, SIZE_MAX, 2, 1));
    memory_arena_destroy(&arena);
}

static void test_alloc_array_valid_returns_non_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NOT_NULL(ctx, memory_arena_alloc_array(&arena, 8, sizeof(i32), 4));
    memory_arena_destroy(&arena);
}

/*
 * alloc_array_zero
 */

static void test_alloc_array_zero_zero_count_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc_array_zero(&arena, 0, 4, 4));
    memory_arena_destroy(&arena);
}

static void test_alloc_array_zero_zero_element_size_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc_array_zero(&arena, 4, 0, 4));
    memory_arena_destroy(&arena);
}

static void test_alloc_array_zero_overflow_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc_array_zero(&arena, SIZE_MAX, 2, 1));
    memory_arena_destroy(&arena);
}

static void test_alloc_array_zero_returns_zeroed_memory(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);

    u8* dirty = (u8*)memory_arena_alloc(&arena, 32, 1);
    if (dirty != NULL)
    {
        memset(dirty, 0xFF, 32);
    }

    memory_arena_reset(&arena);

    u8* mem = (u8*)memory_arena_alloc_array_zero(&arena, 8, sizeof(i32), 4);
    TEST_ASSERT_NOT_NULL(ctx, mem);

    bool all_zero = true;
    for (usize i = 0; i < 8 * sizeof(i32); i++)
    {
        if (mem[i] != 0)
        {
            all_zero = false;
            break;
        }
    }
    TEST_ASSERT_TRUE(ctx, all_zero);

    memory_arena_destroy(&arena);
}

/*
 * reset
 */

static void test_reset_sets_used_to_zero(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_alloc(&arena, 64, 1);
    memory_arena_reset(&arena);
    TEST_ASSERT_INT_EQ(ctx, (int)arena.used, 0);
    memory_arena_destroy(&arena);
}

static void test_reset_allows_reuse(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 64);
    void* first = memory_arena_alloc(&arena, 64, 1);
    memory_arena_reset(&arena);
    void* second = memory_arena_alloc(&arena, 64, 1);
    TEST_ASSERT(ctx, first == second);
    memory_arena_destroy(&arena);
}

static void test_reset_does_not_clear_peak(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_alloc(&arena, 64, 1);
    memory_arena_reset(&arena);
    TEST_ASSERT_INT_EQ(ctx, (int)arena.peak, 64);
    memory_arena_destroy(&arena);
}

/*
 * get_marker / rewind
 */

static void test_get_marker_null_arena_returns_false(TestContext* ctx)
{
    MemoryArenaMarker m;
    TEST_ASSERT_FALSE(ctx, memory_arena_get_marker(NULL, &m));
}

static void test_get_marker_null_out_returns_false(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_FALSE(ctx, memory_arena_get_marker(&arena, NULL));
    memory_arena_destroy(&arena);
}

static void test_get_marker_returns_used(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_alloc(&arena, 32, 1);

    MemoryArenaMarker m;
    TEST_ASSERT_TRUE(ctx, memory_arena_get_marker(&arena, &m));
    TEST_ASSERT_INT_EQ(ctx, (int)m.used, 32);

    memory_arena_destroy(&arena);
}

static void test_rewind_restores_used(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_alloc(&arena, 32, 1);

    MemoryArenaMarker m;
    memory_arena_get_marker(&arena, &m);

    memory_arena_alloc(&arena, 64, 1);
    memory_arena_rewind(&arena, m);

    TEST_ASSERT_INT_EQ(ctx, (int)arena.used, 32);

    memory_arena_destroy(&arena);
}

static void test_rewind_past_used_returns_false(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_alloc(&arena, 32, 1);
    TEST_ASSERT_FALSE(ctx, memory_arena_rewind(&arena, (MemoryArenaMarker){.used = 64}));
    memory_arena_destroy(&arena);
}

static void test_rewind_null_arena_returns_false(TestContext* ctx)
{
    TEST_ASSERT_FALSE(ctx, memory_arena_rewind(NULL, (MemoryArenaMarker){.used = 0}));
}

/*
 * remaining
 */

static void test_remaining_null_returns_zero(TestContext* ctx)
{
    TEST_ASSERT_INT_EQ(ctx, (int)memory_arena_remaining(NULL), 0);
}

static void test_remaining_decreases_by_allocation_size(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    usize before = memory_arena_remaining(&arena);
    memory_arena_alloc(&arena, 64, 1);
    TEST_ASSERT_INT_EQ(ctx, (int)memory_arena_remaining(&arena), (int)(before - 64));
    memory_arena_destroy(&arena);
}

/*
 * remaining_aligned
 */

static void test_remaining_aligned_null_returns_zero(TestContext* ctx)
{
    TEST_ASSERT_INT_EQ(ctx, (int)memory_arena_remaining_aligned(NULL, 16), 0);
}

static void test_remaining_aligned_non_power_of_two_returns_zero(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    TEST_ASSERT_INT_EQ(ctx, (int)memory_arena_remaining_aligned(&arena, 3), 0);
    memory_arena_destroy(&arena);
}

static void test_remaining_aligned_accounts_for_padding(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_alloc(&arena, 1, 1);

    usize unaligned = memory_arena_remaining(&arena);
    usize aligned = memory_arena_remaining_aligned(&arena, 16);

    TEST_ASSERT(ctx, aligned < unaligned);
    TEST_ASSERT_INT_EQ(ctx, (int)aligned, (int)(unaligned - 15));

    memory_arena_destroy(&arena);
}

static void test_remaining_aligned_returns_zero_when_padding_exceeds_remaining(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 17);
    memory_arena_alloc(&arena, 1, 1);
    TEST_ASSERT_INT_EQ(ctx, (int)memory_arena_remaining_aligned(&arena, 32), 0);
    memory_arena_destroy(&arena);
}

/*
 * destroy
 */

static void test_destroy_twice_is_safe(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_destroy(&arena);
    memory_arena_destroy(&arena);
    TEST_ASSERT_NULL(ctx, arena.base);
}

static void test_destroy_then_alloc_returns_null(TestContext* ctx)
{
    MemoryArena arena;
    memory_arena_create(&arena, 1024);
    memory_arena_destroy(&arena);
    TEST_ASSERT_NULL(ctx, memory_arena_alloc(&arena, 16, 16));
}

/*
 * Suite registration
 */

static TestRegistry reg;

static void setup(void)
{
    describe("create")
    {
        test(test_create_null_arena_returns_false);
        test(test_create_zero_capacity_returns_false);
        test(test_create_capacity_overflow_returns_false);
        test(test_create_valid_initializes_fields);
    }
    describe("alloc")
    {
        test(test_alloc_null_arena_returns_null);
        test(test_alloc_zero_size_returns_null);
        test(test_alloc_non_power_of_two_alignment_returns_null);
        test(test_alloc_zero_alignment_returns_null);
        test(test_alloc_alignment_exceeds_max_returns_null);
        test(test_alloc_returns_non_null);
        test(test_alloc_address_is_aligned);
        test(test_alloc_exhausted_arena_returns_null);
        test(test_alloc_updates_used);
        test(test_alloc_updates_peak);
    }
    describe("alloc_zero")
    {
        test(test_alloc_zero_returns_zeroed_memory);
    }
    describe("alloc_array")
    {
        test(test_alloc_array_zero_count_returns_null);
        test(test_alloc_array_zero_element_size_returns_null);
        test(test_alloc_array_overflow_returns_null);
        test(test_alloc_array_valid_returns_non_null);
    }
    describe("alloc_array_zero")
    {
        test(test_alloc_array_zero_zero_count_returns_null);
        test(test_alloc_array_zero_zero_element_size_returns_null);
        test(test_alloc_array_zero_overflow_returns_null);
        test(test_alloc_array_zero_returns_zeroed_memory);
    }
    describe("reset")
    {
        test(test_reset_sets_used_to_zero);
        test(test_reset_allows_reuse);
        test(test_reset_does_not_clear_peak);
    }
    describe("marker_rewind")
    {
        test(test_get_marker_null_arena_returns_false);
        test(test_get_marker_null_out_returns_false);
        test(test_get_marker_returns_used);
        test(test_rewind_restores_used);
        test(test_rewind_past_used_returns_false);
        test(test_rewind_null_arena_returns_false);
    }
    describe("remaining")
    {
        test(test_remaining_null_returns_zero);
        test(test_remaining_decreases_by_allocation_size);
    }
    describe("remaining_aligned")
    {
        test(test_remaining_aligned_null_returns_zero);
        test(test_remaining_aligned_non_power_of_two_returns_zero);
        test(test_remaining_aligned_accounts_for_padding);
        test(test_remaining_aligned_returns_zero_when_padding_exceeds_remaining);
    }
    describe("destroy")
    {
        test(test_destroy_twice_is_safe);
        test(test_destroy_then_alloc_returns_null);
    }
}

const TestSuite memory_arena_suite = {"memory_arena", setup, &reg};
