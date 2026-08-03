#define STB_TRUETYPE_IMPLEMENTATION
#include "core/font.h"

#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_truetype.h"

struct Font
{
    unsigned char* ttf_buffer;
    stbtt_fontinfo info;
    f32 scale;
    int ascent;
};

Font* font_load(const char* path, f32 size_px)
{
    if (path == NULL || size_px <= 0.0f)
    {
        return NULL;
    }

    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size <= 0)
    {
        fclose(file);
        return NULL;
    }

    unsigned char* ttf_buffer = (unsigned char*)malloc((size_t)file_size);
    if (ttf_buffer == NULL)
    {
        fclose(file);
        return NULL;
    }

    if (fread(ttf_buffer, 1, (size_t)file_size, file) != (size_t)file_size)
    {
        free(ttf_buffer);
        fclose(file);
        return NULL;
    }

    fclose(file);

    Font* font = (Font*)malloc(sizeof(Font));
    if (font == NULL)
    {
        free(ttf_buffer);
        return NULL;
    }

    if (!stbtt_InitFont(&font->info, ttf_buffer, 0))
    {
        free(ttf_buffer);
        free(font);
        return NULL;
    }

    font->ttf_buffer = ttf_buffer;
    font->scale = stbtt_ScaleForPixelHeight(&font->info, size_px);

    int descent, line_gap;
    stbtt_GetFontVMetrics(&font->info, &font->ascent, &descent, &line_gap);
    (void)descent;
    (void)line_gap;

    return font;
}

void font_destroy(Font* font)
{
    if (font == NULL)
    {
        return;
    }

    free(font->ttf_buffer);
    free(font);
}

void font_draw_text(Font* font, u32* pixels, i32 width, i32 height, i32 x, i32 y, const char* text,
                    u32 color)
{
    if (font == NULL || pixels == NULL || text == NULL)
    {
        return;
    }

    const u8 cr = (u8)((color >> 16) & 0xFF);
    const u8 cg = (u8)((color >> 8) & 0xFF);
    const u8 cb = (u8)(color & 0xFF);

    const i32 baseline = (i32)((f32)font->ascent * font->scale);

    i32 cursor_x = 0;

    for (const char* p = text; *p != '\0'; p++)
    {
        const int codepoint = (unsigned char)*p;

        int glyph_w, glyph_h, glyph_xoff, glyph_yoff;
        unsigned char* bitmap = stbtt_GetCodepointBitmap(
            &font->info, 0, font->scale, codepoint, &glyph_w, &glyph_h, &glyph_xoff, &glyph_yoff);

        if (bitmap != NULL)
        {
            const i32 dst_x = x + cursor_x + glyph_xoff;
            const i32 dst_y = y + baseline + glyph_yoff;

            for (int gy = 0; gy < glyph_h; gy++)
            {
                const i32 py = dst_y + gy;
                if (py < 0 || py >= height)
                {
                    continue;
                }

                for (int gx = 0; gx < glyph_w; gx++)
                {
                    const i32 px = dst_x + gx;
                    if (px < 0 || px >= width)
                    {
                        continue;
                    }

                    const u8 alpha = bitmap[gy * glyph_w + gx];
                    if (alpha == 0)
                    {
                        continue;
                    }

                    const u32 bg = pixels[py * width + px];
                    const u8 br = (u8)((bg >> 16) & 0xFF);
                    const u8 bgg = (u8)((bg >> 8) & 0xFF);
                    const u8 bb = (u8)(bg & 0xFF);

                    const u32 a = (u32)alpha;
                    const u32 ia = 255u - a;

                    const u8 out_r = (u8)(((u32)cr * a + (u32)br * ia) / 255u);
                    const u8 out_g = (u8)(((u32)cg * a + (u32)bgg * ia) / 255u);
                    const u8 out_b = (u8)(((u32)cb * a + (u32)bb * ia) / 255u);

                    pixels[py * width + px] = ((u32)out_r << 16) | ((u32)out_g << 8) | (u32)out_b;
                }
            }

            stbtt_FreeBitmap(bitmap, NULL);
        }

        int advance_width, left_side_bearing;
        stbtt_GetCodepointHMetrics(&font->info, codepoint, &advance_width, &left_side_bearing);
        (void)left_side_bearing;
        cursor_x += (i32)((f32)advance_width * font->scale);

        if (*(p + 1) != '\0')
        {
            cursor_x += (i32)((f32)stbtt_GetCodepointKernAdvance(&font->info, codepoint,
                                                                 (unsigned char)*(p + 1))
                              * font->scale);
        }
    }
}
