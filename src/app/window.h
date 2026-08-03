#ifndef APP_WINDOW_H
#define APP_WINDOW_H

typedef struct Window Window;

typedef struct WindowConfig
{
    const char* title;
    i32 width;
    i32 height;
    bool resizable;
} WindowConfig;

Window* window_create(const WindowConfig* config);

void window_destroy(Window* window);

void window_poll_events(Window* window);

bool window_should_close(const Window* window);

i32 window_get_width(const Window* window);

i32 window_get_height(const Window* window);

void window_set_title(Window* window, const char* title);

void window_present(Window* window, const u32* pixels, i32 width, i32 height);

#endif // APP_WINDOW_H
