#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include "platform/keyboard.h"

typedef struct Window Window;

typedef struct WindowConfig
{
    const char* title;
    const char* canvas_id;
    i32 width;
    i32 height;
    i32 display_index;
    i32 min_width;
    i32 min_height;
    bool resizable;
    bool fullscreen;
    bool vsync;
} WindowConfig;

Window* window_create(const WindowConfig* config);

void window_destroy(Window* window);

void window_poll_events(Window* window);

bool window_should_close(const Window* window);

i32 window_get_width(const Window* window);

i32 window_get_height(const Window* window);

void window_set_title(Window* window, const char* title);

void window_present(Window* window, const u32* pixels, i32 width, i32 height);

bool window_is_key_down(const Window* window, PlatformKey key);

#endif // APP_WINDOW_H
