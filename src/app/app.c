#include "app/app.h"

#include <stdlib.h>
#include <string.h>

#include "app/window.h"
#include "core/memory_arena.h"
#include "math/vec2f.h"
#include "platform/platform.h"

#define WINDOW_TITLE           "Software Rasterizer"
#define WINDOW_WIDTH           800
#define WINDOW_HEIGHT          600
#define MAX_FRAMEBUFFER_WIDTH  1920
#define MAX_FRAMEBUFFER_HEIGHT 1080
#define MAX_FRAMEBUFFER_PIXELS ((usize)MAX_FRAMEBUFFER_WIDTH * MAX_FRAMEBUFFER_HEIGHT)
#define COLOR_DARK_GREY        0x22222222
#define COLOR_BLUE             0x0000AAFF

typedef struct AppState
{
    Window* window;
    MemoryArena pixels_arena;
    u32* pixels;
    i32 width;
    i32 height;
} AppState;

static inline f32 edge(const Vec2f a, const Vec2f b, const Vec2f p)
{
    return vec2f_cross(vec2f_sub(b, a), vec2f_sub(p, a));
}

static void draw_triangle(u32* pixels, int width, int height, Vec2f v0, Vec2f v1, Vec2f v2,
                          u32 color)
{
    int x_min = (int)fminf(v0.x, fminf(v1.x, v2.x));
    int y_min = (int)fminf(v0.y, fminf(v1.y, v2.y));
    int x_max = (int)fmaxf(v0.x, fmaxf(v1.x, v2.x));
    int y_max = (int)fmaxf(v0.y, fmaxf(v1.y, v2.y));

    x_min = x_min < 0 ? 0 : x_min;
    y_min = y_min < 0 ? 0 : y_min;
    x_max = x_max >= width ? width - 1 : x_max;
    y_max = y_max >= height ? height - 1 : y_max;

    const Vec2f p0 = vec2f((f32)x_min + 0.5f, (f32)y_min + 0.5f);

    const f32 w0_init = edge(v1, v2, p0);
    const f32 w1_init = edge(v2, v0, p0);
    const f32 w2_init = edge(v0, v1, p0);

    const f32 w0_step_x = -(v2.y - v1.y), w0_step_y = v2.x - v1.x;
    const f32 w1_step_x = -(v0.y - v2.y), w1_step_y = v0.x - v2.x;
    const f32 w2_step_x = -(v1.y - v0.y), w2_step_y = v1.x - v0.x;

    f32 row_w0 = w0_init;
    f32 row_w1 = w1_init;
    f32 row_w2 = w2_init;

    for (int y = y_min; y <= y_max; y++)
    {
        f32 w0 = row_w0;
        f32 w1 = row_w1;
        f32 w2 = row_w2;

        bool span_started = false;

        for (int x = x_min; x <= x_max; x++)
        {
            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f)
            {
                pixels[y * width + x] = color;
                span_started = true;
            }
            else if (span_started)
            {
                break;
            }

            w0 += w0_step_x;
            w1 += w1_step_x;
            w2 += w2_step_x;
        }

        row_w0 += w0_step_y;
        row_w1 += w1_step_y;
        row_w2 += w2_step_y;
    }
}

static bool frame(void* arg)
{
    AppState* state = (AppState*)arg;

    window_poll_events(state->window);

    i32 width = window_get_width(state->window);
    i32 height = window_get_height(state->window);

    if (width > MAX_FRAMEBUFFER_WIDTH)
    {
        width = MAX_FRAMEBUFFER_WIDTH;
    }

    if (height > MAX_FRAMEBUFFER_HEIGHT)
    {
        height = MAX_FRAMEBUFFER_HEIGHT;
    }

    if (width != state->width || height != state->height)
    {
        memory_arena_reset(&state->pixels_arena);
        u32* resized = MEM_ARENA_PUSH_ARRAY(&state->pixels_arena, (usize)width * height, u32);
        if (resized == NULL)
        {
            return false;
        }
        state->pixels = resized;
        state->width = width;
        state->height = height;
    }

    const f32 rotation_speed = 1.0f; // radians per second

    f32 angle = (f32)(platform_get_time_seconds() * rotation_speed);
    f32 radius = (f32)(width < height ? width : height) * 0.25f;

    // Move the origin to the center of the triangle
    Vec2f center = vec2f((f32)width * 0.5f, (f32)height * 0.5f);

    f32 a0 = angle;
    f32 a1 = angle + (2.0f * (f32)MATH_PI / 3.0f);
    f32 a2 = angle + (4.0f * (f32)MATH_PI / 3.0f);

    Vec2f base = vec2f(radius, 0.0f);

    Vec2f v0 = vec2f_add(center, vec2f_rotate(base, a0));
    Vec2f v1 = vec2f_add(center, vec2f_rotate(base, a1));
    Vec2f v2 = vec2f_add(center, vec2f_rotate(base, a2));

    // Clear framebuffer using byte-fill.
    memset(state->pixels, COLOR_DARK_GREY & 0xFF, (usize)width * height * sizeof(u32));

    draw_triangle(state->pixels, width, height, v0, v1, v2, COLOR_BLUE);

    window_present(state->window, state->pixels, width, height);

    return !window_should_close(state->window);
}

int app_run(void)
{
    if (!platform_init())
    {
        return 1;
    }

    WindowConfig win_config = {
        .title = WINDOW_TITLE, .width = WINDOW_WIDTH, .height = WINDOW_HEIGHT, .resizable = true};

    Window* window = window_create(&win_config);

    if (window == NULL)
    {
        platform_shutdown();
        return 1;
    }

    // clang-format off
    AppState state = {
        .window       = window,
        .pixels_arena = MEM_ARENA_INIT,
        .pixels       = NULL,
        .width        = WINDOW_WIDTH,
        .height       = WINDOW_HEIGHT
    };
    // clang-format on

    if (!memory_arena_create(&state.pixels_arena, MAX_FRAMEBUFFER_PIXELS * sizeof(u32)))
    {
        window_destroy(window);
        platform_shutdown();
        return 1;
    }

    state.pixels =
        MEM_ARENA_PUSH_ARRAY(&state.pixels_arena, (usize)WINDOW_WIDTH * WINDOW_HEIGHT, u32);
    if (state.pixels == NULL)
    {
        memory_arena_destroy(&state.pixels_arena);
        window_destroy(window);
        platform_shutdown();
        return 1;
    }

    platform_run_main_loop(frame, &state);

    memory_arena_destroy(&state.pixels_arena);
    window_destroy(window);
    platform_shutdown();

    return 0;
}
