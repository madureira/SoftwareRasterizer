#include "app/app.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "app/window.h"
#include "core/config.h"
#include "core/file_io.h"
#include "core/font.h"
#include "core/memory_arena.h"
#include "math/vec2f.h"
#include "platform/platform.h"

#define COLOR_DARK_GREY   0x22222222
#define COLOR_BLUE        0x0000AAFF
#define COLOR_GREEN       0x0000FF00
#define COLOR_RED         0x00FF0000
#define TRIANGLE_COS_120  (-0.5f)                // cos(120deg)
#define TRIANGLE_SIN_120  0.8660254037844386f    // sin(120deg) == sqrt(3)/2
#define TRIANGLE_COS_240  (-0.5f)                // cos(240deg)
#define TRIANGLE_SIN_240  (-0.8660254037844386f) // sin(240deg) == -sqrt(3)/2
#define DEBUG_FONT_SIZE   18.0f
#define DEBUG_FONT_COLOR  COLOR_GREEN
#define DEBUG_FONT_FAMILY "assets/fonts/CONSOLA-Powerline.ttf"

typedef struct AppState
{
    Window* window;
    MemoryArena pixels_arena;
    MemoryArena font_arena;
    u32* pixels;
    i32 width;
    i32 height;
    i32 min_width;
    i32 min_height;
    i32 max_width;
    i32 max_height;
    f64 last_frame_time;
    f64 fps;
    f64 frame_time_ms;
    f64 last_overlay_update;
    char overlay_text[64];
    Font* debug_font;
    bool show_fps;
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

    if (width < state->min_width)
    {
        width = state->min_width;
    }

    if (height < state->min_height)
    {
        height = state->min_height;
    }

    if (width > state->max_width)
    {
        width = state->max_width;
    }

    if (height > state->max_height)
    {
        height = state->max_height;
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

    static const u32 palette[3] = { COLOR_BLUE, COLOR_GREEN, COLOR_RED };

#define GRID_COLS 100
#define GRID_ROWS 50

    const f32 padding = fminf((f32)width, (f32)height) * 0.05f;
    const f32 inner_w = (f32)width - 2.0f * padding;
    const f32 inner_h = (f32)height - 2.0f * padding;
    const f32 cell_w = inner_w / (f32)(GRID_COLS - 1);
    const f32 cell_h = inner_h / (f32)(GRID_ROWS - 1);
    const f32 radius = fminf(cell_w, cell_h) * 0.38f;

    // Byte-fill only works because all bytes of COLOR_DARK_GREY are equal (0x22).
    memset(state->pixels, COLOR_DARK_GREY & 0xFF, (usize)width * height * sizeof(u32));

    for (i32 row = 0; row < GRID_ROWS; row++)
    {
        for (i32 col = 0; col < GRID_COLS; col++)
        {
            // Phase offset per cell creates a diagonal wave effect.
            const f32 angle = (f32)now + (f32)col * 0.1f + (f32)row * 0.2f;

            const Vec2f center = vec2f(padding + (f32)col * cell_w, padding + (f32)row * cell_h);
            const Vec2f base = vec2f(radius, 0.0f);

            const f32 s0 = sinf(angle);
            const f32 c0 = cosf(angle);
            const f32 s1 = s0 * TRIANGLE_COS_120 + c0 * TRIANGLE_SIN_120;
            const f32 c1 = c0 * TRIANGLE_COS_120 - s0 * TRIANGLE_SIN_120;
            const f32 s2 = s0 * TRIANGLE_COS_240 + c0 * TRIANGLE_SIN_240;
            const f32 c2 = c0 * TRIANGLE_COS_240 - s0 * TRIANGLE_SIN_240;

            const Vec2f v0 = vec2f_add(center, vec2f_rotate_sincos(base, s0, c0));
            const Vec2f v1 = vec2f_add(center, vec2f_rotate_sincos(base, s1, c1));
            const Vec2f v2 = vec2f_add(center, vec2f_rotate_sincos(base, s2, c2));

            draw_triangle(state->pixels, width, height, v0, v1, v2,
                          palette[(row * GRID_COLS + col) % 3]);
        }
    }

#undef GRID_COLS
#undef GRID_ROWS

    if (state->show_fps)
    {
        draw_debug_overlay(state, now, width, height);
    }

    window_present(state->window, state->pixels, width, height);

    return !window_should_close(state->window);
}

int app_start(void)
{
    Config config;

    if (!config_load(&config, "config.ini"))
    {
        return 1;
    }

    if (!platform_init())
    {
        return 1;
    }

    WindowConfig win_config = { .title = config.window_title,
                                .canvas_id = config.canvas_id,
                                .width = config.window_width,
                                .height = config.window_height,
                                .display_index = config.window_display_index,
                                .min_width = config.window_min_width,
                                .min_height = config.window_min_height,
                                .resizable = config.resizable,
                                .fullscreen = config.fullscreen,
                                .vsync = config.vsync };

    Window* window = window_create(&win_config);

    if (window == NULL)
    {
        platform_shutdown();
        return 1;
    }

    AppState state = { .window = window,
                       .pixels_arena = MEM_ARENA_INIT,
                       .font_arena = MEM_ARENA_INIT,
                       .pixels = NULL,
                       .width = config.window_width,
                       .height = config.window_height,
                       .min_width = config.window_min_width,
                       .min_height = config.window_min_height,
                       .max_width = config.window_max_width,
                       .max_height = config.window_max_height,
                       .debug_font = NULL,
                       .show_fps = config.show_fps };

    const u32 max_framebuffer_pixels = config.window_max_width * config.window_max_height;

    if (!memory_arena_create(&state.pixels_arena, max_framebuffer_pixels * sizeof(u32)))
    {
        window_destroy(window);
        platform_shutdown();
        return 1;
    }

    long font_file_size = file_io_size(DEBUG_FONT_FAMILY);

    usize font_arena_size = font_required_memory((usize)font_file_size, DEBUG_FONT_SIZE);
    if (font_arena_size == 0 || !memory_arena_create(&state.font_arena, font_arena_size))
    {
        memory_arena_destroy(&state.pixels_arena);
        window_destroy(window);
        platform_shutdown();
        return 1;
    }

    state.pixels = MEM_ARENA_PUSH_ARRAY(&state.pixels_arena,
                                        (usize)config.window_width * config.window_height, u32);
    if (state.pixels == NULL)
    {
        memory_arena_destroy(&state.pixels_arena);
        window_destroy(window);
        platform_shutdown();
        return 1;
    }

    state.last_frame_time = platform_get_time_seconds();

    state.debug_font =
        font_load(&state.font_arena, DEBUG_FONT_FAMILY, DEBUG_FONT_SIZE, (usize)font_file_size);

    platform_run_main_loop(frame, &state, config.target_fps);

    memory_arena_destroy(&state.font_arena);
    memory_arena_destroy(&state.pixels_arena);
    window_destroy(window);
    platform_shutdown();

    return 0;
}
