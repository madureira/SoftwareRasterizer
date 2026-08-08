#include "app/app.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "app/window.h"
#include "core/config.h"
#include "core/file_io.h"
#include "core/font.h"
#include "core/log.h"
#include "core/memory_arena.h"
#include "core/profiler.h"
#include "core/simd.h"
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
#define GRID_COLS         20
#define GRID_ROWS         10
#define GRID_MIN          2
#define GRID_MAX          50
#define ROT_SPEED_INIT    1.0f
#define ROT_ACCEL         3.0f
#define ROT_SPEED_MAX     20.0f

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
    i32 grid_cols;
    i32 grid_rows;
    bool key_up_prev;
    bool key_down_prev;
    f64 rotation;
    f32 rot_speed;
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

#if SIMD_SSE2
    const __m128 lane_offsets = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);

    const __m128 bump0 = _mm_mul_ps(_mm_set1_ps(w0_step_x), lane_offsets);
    const __m128 bump1 = _mm_mul_ps(_mm_set1_ps(w1_step_x), lane_offsets);
    const __m128 bump2 = _mm_mul_ps(_mm_set1_ps(w2_step_x), lane_offsets);

    const f32 advance0 = w0_step_x * (f32)SIMD_LANES;
    const f32 advance1 = w1_step_x * (f32)SIMD_LANES;
    const f32 advance2 = w2_step_x * (f32)SIMD_LANES;

    const __m128 zero = _mm_setzero_ps();
#elif SIMD_NEON
    static const f32 offsets_arr[4] = { 0.0f, 1.0f, 2.0f, 3.0f };
    const float32x4_t lane_offsets = vld1q_f32(offsets_arr);
    const float32x4_t bump0 = vmulq_f32(vdupq_n_f32(w0_step_x), lane_offsets);
    const float32x4_t bump1 = vmulq_f32(vdupq_n_f32(w1_step_x), lane_offsets);
    const float32x4_t bump2 = vmulq_f32(vdupq_n_f32(w2_step_x), lane_offsets);
    const f32 advance0 = w0_step_x * (f32)SIMD_LANES;
    const f32 advance1 = w1_step_x * (f32)SIMD_LANES;
    const f32 advance2 = w2_step_x * (f32)SIMD_LANES;
    const float32x4_t zero = vdupq_n_f32(0.0f);
    static const u32 bit_weights_arr[4] = { 1, 2, 4, 8 };
    const uint32x4_t bit_weights = vld1q_u32(bit_weights_arr);
#endif

    f32 row_w0 = w0_init;
    f32 row_w1 = w1_init;
    f32 row_w2 = w2_init;

    for (int y = y_min; y <= y_max; y++)
    {
        f32 w0 = row_w0;
        f32 w1 = row_w1;
        f32 w2 = row_w2;

        bool span_started = false;
        int x = x_min;

#if SIMD_SSE2
        for (; x <= x_max - (SIMD_LANES - 1); x += SIMD_LANES)
        {
            __m128 w0_vec = _mm_add_ps(_mm_set1_ps(w0), bump0);
            __m128 w1_vec = _mm_add_ps(_mm_set1_ps(w1), bump1);
            __m128 w2_vec = _mm_add_ps(_mm_set1_ps(w2), bump2);

            __m128 inside =
                _mm_and_ps(_mm_and_ps(_mm_cmpge_ps(w0_vec, zero), _mm_cmpge_ps(w1_vec, zero)),
                           _mm_cmpge_ps(w2_vec, zero));

            int mask = _mm_movemask_ps(inside);

            if (mask != 0)
            {
                u32* dst = &pixels[y * width + x];
                if (mask & 1)
                {
                    dst[0] = color;
                }
                if (mask & 2)
                {
                    dst[1] = color;
                }
                if (mask & 4)
                {
                    dst[2] = color;
                }
                if (mask & 8)
                {
                    dst[3] = color;
                }
                span_started = true;
            }
            else if (span_started)
            {
                break;
            }

            w0 += advance0;
            w1 += advance1;
            w2 += advance2;
        }
#elif SIMD_NEON
        for (; x <= x_max - (SIMD_LANES - 1); x += SIMD_LANES)
        {
            float32x4_t w0_vec = vaddq_f32(vdupq_n_f32(w0), bump0);
            float32x4_t w1_vec = vaddq_f32(vdupq_n_f32(w1), bump1);
            float32x4_t w2_vec = vaddq_f32(vdupq_n_f32(w2), bump2);

            uint32x4_t inside =
                vandq_u32(vandq_u32(vcgeq_f32(w0_vec, zero), vcgeq_f32(w1_vec, zero)),
                          vcgeq_f32(w2_vec, zero));

            uint32x4_t bits = vshrq_n_u32(inside, 31);
            uint32x4_t weighted = vmulq_u32(bits, bit_weights);
            int mask = (int)vaddvq_u32(weighted);

            if (mask != 0)
            {
                u32* dst = &pixels[y * width + x];
                if (mask & 1)
                {
                    dst[0] = color;
                }
                if (mask & 2)
                {
                    dst[1] = color;
                }
                if (mask & 4)
                {
                    dst[2] = color;
                }
                if (mask & 8)
                {
                    dst[3] = color;
                }
                span_started = true;
            }
            else if (span_started)
            {
                break;
            }

            w0 += advance0;
            w1 += advance1;
            w2 += advance2;
        }
#endif
        // Scalar loop: processes all pixels when SIMD is ON, or the remaining tail when it is.
        for (; x <= x_max; x++)
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
    ProfilerToken frame_profile = PROFILE_BEGIN("frame", "Frame");

    AppState* state = (AppState*)arg;

    const f64 now = platform_get_time_seconds();
    const f64 dt = now - state->last_frame_time;
    state->last_frame_time = now;

    if (dt > 0.0)
    {
        state->fps = 1.0 / dt;
        state->frame_time_ms = dt * 1000.0;
    }

    ProfilerToken poll_profile = PROFILE_BEGIN("platform", "Poll Events");
    window_poll_events(state->window);
    PROFILE_END(poll_profile);

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

    const bool key_up = window_is_key_down(state->window, PLATFORM_KEY_UP);
    const bool key_down = window_is_key_down(state->window, PLATFORM_KEY_DOWN);

    if (key_up && /*!state->key_up_prev &&*/ state->grid_cols < GRID_MAX
        && state->grid_rows < GRID_MAX)
    {
        state->grid_cols++;
        state->grid_rows++;
    }
    if (key_down && /*!state->key_down_prev &&*/ state->grid_cols > GRID_MIN
        && state->grid_rows > GRID_MIN)
    {
        state->grid_cols--;
        state->grid_rows--;
    }

    state->key_up_prev = key_up;
    state->key_down_prev = key_down;

    const bool key_left = window_is_key_down(state->window, PLATFORM_KEY_LEFT);
    const bool key_right = window_is_key_down(state->window, PLATFORM_KEY_RIGHT);

    if (key_right && !key_left)
    {
        if (state->rot_speed < 0.0f)
        {
            state->rot_speed = ROT_SPEED_INIT;
        }
        else
        {
            state->rot_speed += ROT_ACCEL * (f32)dt;
            if (state->rot_speed > ROT_SPEED_MAX)
            {
                state->rot_speed = ROT_SPEED_MAX;
            }
        }
    }
    else if (key_left && !key_right)
    {
        if (state->rot_speed > 0.0f)
        {
            state->rot_speed = -ROT_SPEED_INIT;
        }
        else
        {
            state->rot_speed -= ROT_ACCEL * (f32)dt;
            if (state->rot_speed < -ROT_SPEED_MAX)
            {
                state->rot_speed = -ROT_SPEED_MAX;
            }
        }
    }

    state->rotation += state->rot_speed * dt;

    const int grid_cols = state->grid_cols;
    const int grid_rows = state->grid_rows;

    const f32 padding = fminf((f32)width, (f32)height) * 0.05f;
    const f32 inner_w = (f32)width - 2.0f * padding;
    const f32 inner_h = (f32)height - 2.0f * padding;
    const f32 cell_w = inner_w / (f32)(grid_cols - 1);
    const f32 cell_h = inner_h / (f32)(grid_rows - 1);
    const f32 radius = fminf(cell_w, cell_h) * 0.80f;

    ProfilerToken clear_profile = PROFILE_BEGIN("render", "Clear");
    // Byte-fill only works because all bytes of COLOR_DARK_GREY are equal (0x22).
    memset(state->pixels, COLOR_DARK_GREY & 0xFF, (usize)width * height * sizeof(u32));
    PROFILE_END(clear_profile);

    ProfilerToken triangles_profile = PROFILE_BEGIN("render", "Draw Triangles");
    for (i32 row = 0; row < grid_rows; row++)
    {
        for (i32 col = 0; col < grid_cols; col++)
        {
            // Phase offset per cell creates a diagonal wave effect.
            const f32 angle = (f32)state->rotation + (f32)col * 0.1f + (f32)row * 0.2f;

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
    PROFILE_END(triangles_profile);

    if (state->show_fps)
    {
        ProfilerToken overlay_profile = PROFILE_BEGIN("render", "Overlay");
        draw_debug_overlay(state, now, width, height);
        PROFILE_END(overlay_profile);
    }

    ProfilerToken present_profile = PROFILE_BEGIN("platform", "Present");

    window_present(state->window, state->pixels, width, height);

    PROFILE_END(present_profile);

    PROFILE_END(frame_profile);

    return !window_should_close(state->window);
}

int app_start(void)
{
    Config config;

    if (!config_load(&config, "config.ini"))
    {
        LOG_ERROR("Failed to load config.ini");
        return EXIT_FAILURE;
    }

    if (!platform_init())
    {
        LOG_ERROR("Failed to init platform");
        return EXIT_FAILURE;
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
        LOG_ERROR("Failed to create window \"%s\" (%dx%d)", config.window_title,
                  config.window_width, config.window_height);
        platform_shutdown();
        return EXIT_FAILURE;
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
                       .show_fps = config.show_fps,
                       .grid_cols = GRID_COLS,
                       .grid_rows = GRID_ROWS,
                       .rot_speed = -ROT_SPEED_INIT };

    const u32 max_framebuffer_pixels = config.window_max_width * config.window_max_height;

    if (!memory_arena_create(&state.pixels_arena, max_framebuffer_pixels * sizeof(u32)))
    {
        LOG_ERROR("Failed to create pixels arena (%u bytes)",
                  max_framebuffer_pixels * (u32)sizeof(u32));
        window_destroy(window);
        platform_shutdown();
        return EXIT_FAILURE;
    }

    long font_file_size = file_io_size(DEBUG_FONT_FAMILY);

    usize font_arena_size = font_required_memory((usize)font_file_size, DEBUG_FONT_SIZE);
    if (font_arena_size == 0 || !memory_arena_create(&state.font_arena, font_arena_size))
    {
        LOG_ERROR("Failed to create font arena for \"%s\" (size %.1f, %zu bytes)",
                  DEBUG_FONT_FAMILY, (double)DEBUG_FONT_SIZE, font_arena_size);
        memory_arena_destroy(&state.pixels_arena);
        window_destroy(window);
        platform_shutdown();
        return EXIT_FAILURE;
    }

    state.pixels = MEM_ARENA_PUSH_ARRAY(&state.pixels_arena,
                                        (usize)config.window_width * config.window_height, u32);
    if (state.pixels == NULL)
    {
        LOG_ERROR("Failed to allocate framebuffer (%dx%d)", config.window_width,
                  config.window_height);
        memory_arena_destroy(&state.font_arena);
        memory_arena_destroy(&state.pixels_arena);
        window_destroy(window);
        platform_shutdown();
        return EXIT_FAILURE;
    }

    state.last_frame_time = platform_get_time_seconds();

    state.debug_font =
        font_load(&state.font_arena, DEBUG_FONT_FAMILY, DEBUG_FONT_SIZE, (usize)font_file_size);

    if (state.debug_font == NULL)
    {
        LOG_WARN("Failed to load debug font \"%s\" (size %.1f) — overlay disabled",
                 DEBUG_FONT_FAMILY, (double)DEBUG_FONT_SIZE);
    }

    PROFILE_INIT();

    platform_run_main_loop(frame, &state, config.target_fps);

    if (!PROFILE_WRITE("trace.json"))
    {
        LOG_ERROR("Failed to write profiling trace to \"trace.json\"");
    }

    memory_arena_destroy(&state.font_arena);
    memory_arena_destroy(&state.pixels_arena);
    window_destroy(window);
    platform_shutdown();

    return EXIT_SUCCESS;
}
