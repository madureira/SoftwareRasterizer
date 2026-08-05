#define STB_TRUETYPE_IMPLEMENTATION
#include "core/font.h"

#include "core/file_io.h"
#include "core/memory_arena.h"
#include "stb/stb_truetype.h"

#define GLYPH_CACHE_FIRST 32
#define GLYPH_CACHE_LAST  126
#define GLYPH_CACHE_COUNT (GLYPH_CACHE_LAST - GLYPH_CACHE_FIRST + 1)

typedef struct GlyphEntry
{
    u8* bitmap;
    i32 width;
    i32 height;
    i32 offset_x;
    i32 offset_y;
    i32 advance;
} GlyphEntry;

struct Font
{
    stbtt_fontinfo info;
    f32 scale;
    i32 ascent;
    GlyphEntry glyphs[GLYPH_CACHE_COUNT];
};

usize font_required_memory(usize file_size, f32 size_px)
{
    if (file_size == 0 || size_px <= 0.0f)
    {
        return 0;
    }

    usize glyph_side = (usize)(size_px + 1.0f);
    usize bitmap_budget = (usize)GLYPH_CACHE_COUNT * glyph_side * glyph_side;

    // Absorbs per-allocation alignment padding from the arena (~64 bytes * 97 allocations)
    usize alignment_overhead = (usize)GLYPH_CACHE_COUNT * MEMORY_ARENA_MAX_ALIGNMENT;

    return file_size + sizeof(Font) + bitmap_budget + alignment_overhead;
}

Font* font_load(MemoryArena* arena, const char* path, f32 size_px, usize file_size)
{
    if (arena == NULL || path == NULL || size_px <= 0.0f || file_size == 0)
    {
        return NULL;
    }

    u8* ttf_buffer = MEM_ARENA_PUSH_ARRAY(arena, file_size, u8);
    if (ttf_buffer == NULL)
    {
        return NULL;
    }

    if (!file_io_read(path, ttf_buffer))
    {
        return NULL;
    }

    Font* font = MEM_ARENA_PUSH_STRUCT_ZERO(arena, Font);
    if (font == NULL)
    {
        return NULL;
    }

    if (!stbtt_InitFont(&font->info, ttf_buffer, 0))
    {
        return NULL;
    }

    font->scale = stbtt_ScaleForPixelHeight(&font->info, size_px);

    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&font->info, &ascent, &descent, &line_gap);
    font->ascent = ascent;
    (void)line_gap;

    for (int cp = GLYPH_CACHE_FIRST; cp <= GLYPH_CACHE_LAST; cp++)
    {
        GlyphEntry* g = &font->glyphs[cp - GLYPH_CACHE_FIRST];

        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&font->info, cp, font->scale, font->scale, &x0, &y0, &x1, &y1);

        g->width = x1 - x0;
        g->height = y1 - y0;
        g->offset_x = x0;
        g->offset_y = y0;

        int advance_width, left_side_bearing;
        stbtt_GetCodepointHMetrics(&font->info, cp, &advance_width, &left_side_bearing);
        g->advance = (i32)((f32)advance_width * font->scale);
        (void)left_side_bearing;

        if (g->width > 0 && g->height > 0)
        {
            g->bitmap = MEM_ARENA_PUSH_ARRAY(arena, (usize)(g->width * g->height), u8);
            if (g->bitmap != NULL)
            {
                stbtt_MakeCodepointBitmap(&font->info, g->bitmap, g->width, g->height, g->width,
                                          font->scale, font->scale, cp);
            }
        }
    }

    return font;
}

void font_draw_text(Font* font, u32* pixels, i32 width, i32 height, i32 x, i32 y, const char* text,
                    u32 color)
{
    if (font == NULL || pixels == NULL || text == NULL)
    {
        return;
    }

    const u32 cr = (color >> 16) & 0xFF;
    const u32 cg = (color >> 8) & 0xFF;
    const u32 cb = color & 0xFF;

    const i32 baseline = (i32)((f32)font->ascent * font->scale);

    i32 cursor_x = 0;

    for (const char* p = text; *p != '\0'; p++)
    {
        const int codepoint = (unsigned char)*p;

        if (codepoint < GLYPH_CACHE_FIRST || codepoint > GLYPH_CACHE_LAST)
        {
            continue;
        }

        const GlyphEntry* g = &font->glyphs[codepoint - GLYPH_CACHE_FIRST];

        if (g->bitmap != NULL)
        {
            const i32 dst_x = x + cursor_x + g->offset_x;
            const i32 dst_y = y + baseline + g->offset_y;

            const i32 gy_min = dst_y < 0 ? -dst_y : 0;
            const i32 gy_max = (dst_y + g->height > height) ? height - dst_y : g->height;
            const i32 gx_min = dst_x < 0 ? -dst_x : 0;
            const i32 gx_max = (dst_x + g->width > width) ? width - dst_x : g->width;

            for (i32 gy = gy_min; gy < gy_max; gy++)
            {
                const i32 py = dst_y + gy;

                for (i32 gx = gx_min; gx < gx_max; gx++)
                {
                    const u32 alpha = g->bitmap[gy * g->width + gx];
                    if (alpha == 0)
                    {
                        continue;
                    }

                    const i32 px = dst_x + gx;
                    const u32 bg = pixels[py * width + px];
                    const u32 br = (bg >> 16) & 0xFF;
                    const u32 bgg = (bg >> 8) & 0xFF;
                    const u32 bb = bg & 0xFF;

                    const u32 ia = 255u - alpha;
                    const u8 out_r = (u8)((cr * alpha + br * ia) >> 8);
                    const u8 out_g = (u8)((cg * alpha + bgg * ia) >> 8);
                    const u8 out_b = (u8)((cb * alpha + bb * ia) >> 8);

                    pixels[py * width + px] = ((u32)out_r << 16) | ((u32)out_g << 8) | (u32)out_b;
                }
            }
        }

        cursor_x += g->advance;

        if (*(p + 1) != '\0')
        {
            cursor_x += (i32)((f32)stbtt_GetCodepointKernAdvance(&font->info, codepoint,
                                                                 (unsigned char)*(p + 1))
                              * font->scale);
        }
    }
}
