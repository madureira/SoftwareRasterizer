#include "app/app.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "app/window.h"
#include "core/font.h"
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
#define COLOR_GREEN            0x0000FF00
#define DEBUG_FONT_SIZE        24.0f
#define DEBUG_FONT_COLOR       COLOR_GREEN
#define TRIANGLE_COS_120       (-0.5f)                // cos(120deg)
#define TRIANGLE_SIN_120       0.8660254037844386f    // sin(120deg) == sqrt(3)/2
#define TRIANGLE_COS_240       (-0.5f)                // cos(240deg)
#define TRIANGLE_SIN_240       (-0.8660254037844386f) // sin(240deg) == -sqrt(3)/2

typedef struct AppState
{
    Window* window;
    MemoryArena pixels_arena;
    u32* pixels;
    i32 width;
    i32 height;
    f64 last_frame_time;
    f64 fps;
    f64 frame_time_ms;
    f64 last_overlay_update;
    char overlay_text[64];
    Font* debug_font;
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

static inline void draw_debug_overlay(AppState* state, f64 now, i32 width, i32 height)
{
    if (now - state->last_overlay_update >= 1.0)
    {
        snprintf(state->overlay_text, sizeof(state->overlay_text), "FPS: %.2f - %.2fms", state->fps,
                 state->frame_time_ms);
        state->last_overlay_update = now;
    }

    if (state->debug_font != NULL)
    {
        font_draw_text(state->debug_font, state->pixels, width, height, 4, 4, state->overlay_text,
                       DEBUG_FONT_COLOR);
    }
}

static bool frame(void* arg)
{
    AppState* state = (AppState*)arg;

    const f64 now = platform_get_time_seconds();
    const f64 dt = now - state->last_frame_time;
    state->last_frame_time = now;

    if (dt > 0.0)
    {
        state->fps = 1.0 / dt;
        state->frame_time_ms = dt * 1000.0;
    }

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

    f32 angle = (f32)(now * rotation_speed);
    f32 radius = (f32)(width < height ? width : height) * 0.25f;

    // Move the origin to the center of the triangle
    Vec2f center = vec2f((f32)width * 0.5f, (f32)height * 0.5f);

    Vec2f base = vec2f(radius, 0.0f);

    const f32 s0 = sinf(angle);
    const f32 c0 = cosf(angle);
    const f32 s1 = s0 * TRIANGLE_COS_120 + c0 * TRIANGLE_SIN_120;
    const f32 c1 = c0 * TRIANGLE_COS_120 - s0 * TRIANGLE_SIN_120;
    const f32 s2 = s0 * TRIANGLE_COS_240 + c0 * TRIANGLE_SIN_240;
    const f32 c2 = c0 * TRIANGLE_COS_240 - s0 * TRIANGLE_SIN_240;

    Vec2f v0 = vec2f_add(center, vec2f_rotate_sincos(base, s0, c0));
    Vec2f v1 = vec2f_add(center, vec2f_rotate_sincos(base, s1, c1));
    Vec2f v2 = vec2f_add(center, vec2f_rotate_sincos(base, s2, c2));

    // Clear framebuffer using byte-fill.
    memset(state->pixels, COLOR_DARK_GREY & 0xFF, (usize)width * height * sizeof(u32));

    draw_triangle(state->pixels, width, height, v0, v1, v2, COLOR_BLUE);

    draw_debug_overlay(state, now, width, height);

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
        .height       = WINDOW_HEIGHT,
        .debug_font   = NULL
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

    state.last_frame_time = platform_get_time_seconds();
    state.debug_font = font_load("assets/fonts/TerminessNerdFont-Regular.ttf", DEBUG_FONT_SIZE);

    platform_run_main_loop(frame, &state);

    font_destroy(state.debug_font);
    memory_arena_destroy(&state.pixels_arena);
    window_destroy(window);
    platform_shutdown();

    return 0;
}
