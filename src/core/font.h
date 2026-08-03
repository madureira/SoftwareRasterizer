#ifndef CORE_FONT_H
#define CORE_FONT_H

typedef struct Font Font;

Font* font_load(const char* path, f32 size_px);

void font_destroy(Font* font);

void font_draw_text(Font* font, u32* pixels, i32 width, i32 height, i32 x, i32 y, const char* text,
                    u32 color);

#endif // CORE_FONT_H
