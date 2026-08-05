#ifndef CORE_FONT_H
#define CORE_FONT_H

#include "core/memory_arena.h"

typedef struct Font Font;

usize font_required_memory(usize file_size, f32 size_px);

Font* font_load(MemoryArena* arena, const char* path, f32 size_px, usize file_size);

void font_draw_text(Font* font, u32* pixels, i32 width, i32 height, i32 x, i32 y, const char* text,
                    u32 color);

#endif // CORE_FONT_H
