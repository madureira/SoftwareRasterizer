#include "core/fps.h"

#include <stdio.h>
#include <stdlib.h>

#include "core/file_io.h"
#include "core/font.h"
#include "core/log.h"
#include "core/memory_arena.h"
#include "platform/platform.h"

#define DEBUG_FONT_SIZE        18.0f
#define DEBUG_FONT_LINE_HEIGHT ((i32)(DEBUG_FONT_SIZE * 1.3f))
#define DEBUG_FONT_COLOR       0x0000FF00
#define DEBUG_FONT_FAMILY      "assets/fonts/CONSOLA-Powerline.ttf"
#define DEBUG_OVERLAY_MARGIN   4
#define DEBUG_OVERLAY_PADDING  6
#define DEBUG_OVERLAY_OPACITY  0.7f

struct FpsCounter
{
    MemoryArena font_arena;
    Font* debug_font;
    f64 fps;
    f64 frame_time_ms;
    f64 render_fps;
    f64 render_time_ms;
    f64 render_time_accum;
    f64 last_overlay_update;
    f64 last_frame_time;
    u64 render_start_counter;
    u64 perf_frequency;
    i32 render_frame_count;
    bool enabled;
    char overlay_text[64];
    char overlay_text2[64];
};

FpsCounter* fps_create(bool enabled)
{
    FpsCounter* fps = malloc(sizeof(*fps));
    if (fps == NULL)
    {
        return NULL;
    }

    *fps = (FpsCounter){ .enabled = enabled,
                         .font_arena = MEM_ARENA_INIT,
                         .last_frame_time = platform_get_time_seconds(),
                         .perf_frequency = platform_get_perf_frequency() };

    if (!enabled)
    {
        return fps;
    }

    const long font_file_size = file_io_size(DEBUG_FONT_FAMILY);
    const usize font_arena_size = font_required_memory((usize)font_file_size, DEBUG_FONT_SIZE);

    if (font_arena_size == 0 || !memory_arena_create(&fps->font_arena, font_arena_size))
    {
        LOG_ERROR("Failed to create font arena for \"%s\" (size %.1f, %zu bytes)",
                  DEBUG_FONT_FAMILY, (double)DEBUG_FONT_SIZE, font_arena_size);
        free(fps);
        return NULL;
    }

    fps->debug_font =
        font_load(&fps->font_arena, DEBUG_FONT_FAMILY, DEBUG_FONT_SIZE, (usize)font_file_size);

    if (fps->debug_font == NULL)
    {
        LOG_WARN("Failed to load debug font \"%s\" (size %.1f) — overlay disabled",
                 DEBUG_FONT_FAMILY, (double)DEBUG_FONT_SIZE);
    }

    return fps;
}

void fps_destroy(FpsCounter* fps)
{
    if (fps == NULL)
    {
        return;
    }

    memory_arena_destroy(&fps->font_arena);
    free(fps);
}

void fps_frame_tick(FpsCounter* fps)
{
    if (fps == NULL || !fps->enabled)
    {
        return;
    }

    const f64 now = platform_get_time_seconds();
    const f64 dt = now - fps->last_frame_time;
    fps->last_frame_time = now;

    if (dt <= 0.0)
    {
        return;
    }

    fps->fps = 1.0 / dt;
    fps->frame_time_ms = dt * 1000.0;
}

void fps_begin_render(FpsCounter* fps)
{
    if (fps == NULL || !fps->enabled)
    {
        return;
    }

    fps->render_start_counter = platform_get_perf_counter();
}

static void draw_overlay_background(u32* pixels, i32 width, i32 height, i32 x, i32 y, i32 w, i32 h,
                                    f32 opacity)
{
    const i32 x_min = x < 0 ? 0 : x;
    const i32 y_min = y < 0 ? 0 : y;
    const i32 x_max = (x + w > width) ? width : x + w;
    const i32 y_max = (y + h > height) ? height : y + h;

    const u32 inv_alpha = 255u - (u32)(opacity * 255.0f + 0.5f);

    for (i32 py = y_min; py < y_max; py++)
    {
        u32* row = &pixels[(usize)py * width];
        for (i32 px = x_min; px < x_max; px++)
        {
            const u32 bg = row[px];
            const u32 br = (bg >> 16) & 0xFF;
            const u32 bg_g = (bg >> 8) & 0xFF;
            const u32 bb = bg & 0xFF;

            const u8 out_r = (u8)((br * inv_alpha) >> 8);
            const u8 out_g = (u8)((bg_g * inv_alpha) >> 8);
            const u8 out_b = (u8)((bb * inv_alpha) >> 8);

            row[px] = ((u32)out_r << 16) | ((u32)out_g << 8) | (u32)out_b;
        }
    }
}

void fps_show(FpsCounter* fps, u32* pixels, i32 width, i32 height)
{
    if (fps == NULL || !fps->enabled || fps->debug_font == NULL)
    {
        return;
    }

    const u64 now_counter = platform_get_perf_counter();
    const f64 render_dt = (f64)(now_counter - fps->render_start_counter) / (f64)fps->perf_frequency;
    fps->render_time_accum += render_dt;
    fps->render_frame_count++;

    const f64 now = platform_get_time_seconds();

    if (now - fps->last_overlay_update >= 1.0)
    {
        if (fps->render_frame_count > 0)
        {
            const f64 avg_render_dt = fps->render_time_accum / (f64)fps->render_frame_count;
            fps->render_fps = 1.0 / avg_render_dt;
            fps->render_time_ms = avg_render_dt * 1000.0;
        }
        fps->render_time_accum = 0.0;
        fps->render_frame_count = 0;

        snprintf(fps->overlay_text, sizeof(fps->overlay_text), "Present: %.2f fps - %.2fms",
                 fps->fps, fps->frame_time_ms);
        snprintf(fps->overlay_text2, sizeof(fps->overlay_text2), "Render:  %.2f fps - %.2fms",
                 fps->render_fps, fps->render_time_ms);
        fps->last_overlay_update = now;
    }

    const i32 text_w1 = font_measure_text(fps->debug_font, fps->overlay_text);
    const i32 text_w2 = font_measure_text(fps->debug_font, fps->overlay_text2);
    const i32 text_w = text_w1 > text_w2 ? text_w1 : text_w2;

    const i32 rect_x = DEBUG_OVERLAY_MARGIN;
    const i32 rect_y = DEBUG_OVERLAY_MARGIN;
    const i32 rect_w = text_w + 2 * DEBUG_OVERLAY_PADDING;
    const i32 rect_h = 2 * DEBUG_FONT_LINE_HEIGHT + 2 * DEBUG_OVERLAY_PADDING;

    draw_overlay_background(pixels, width, height, rect_x, rect_y, rect_w, rect_h,
                            DEBUG_OVERLAY_OPACITY);

    const i32 text_x = rect_x + DEBUG_OVERLAY_PADDING;
    const i32 text_y = rect_y + DEBUG_OVERLAY_PADDING;

    font_draw_text(fps->debug_font, pixels, width, height, text_x, text_y, fps->overlay_text,
                   DEBUG_FONT_COLOR);
    font_draw_text(fps->debug_font, pixels, width, height, text_x, text_y + DEBUG_FONT_LINE_HEIGHT,
                   fps->overlay_text2, DEBUG_FONT_COLOR);
}
